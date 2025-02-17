#include "core/event.h"
#include "gtest/gtest.h"
#include "core/logging.h"

struct TestEvent  {};

struct DataEvent {
    DataEvent(std::vector<std::string_view> strings, bool val, double scale) 
        : m_Strings(strings),
        m_Value(val),
        m_Scale(scale)
        {}

    std::vector<std::string_view> m_Strings;
    bool m_Value;
    double m_Scale;
};

TEST(Event, Init) {
    EventPublisher ep;
    EXPECT_EQ(ep.SubscriberCount(), 0);
    EXPECT_EQ(ep.EventCount(), 0);

    EventSubscriber es;
    EXPECT_EQ(es.SubscribedCount(), 0);
}
TEST(Event, PublishSync) {
    EventPublisher ep;
    EXPECT_EQ(ep.SubscriberCount(), 0);
    EXPECT_EQ(ep.EventCount(), 0);

    int count = 0;

    EventSubscriber es;
    EXPECT_EQ(es.SubscribedCount(), 0);

    EXPECT_EQ(count, 0);
    es.SubscribeTo<TestEvent>(ep, [&count](const TestEvent& e) { ++count; });
    EXPECT_EQ(count, 0);
    EXPECT_EQ(es.SubscribedCount(), 1);
    EXPECT_EQ(ep.SubscriberCount(), 1);
    EXPECT_EQ(ep.EventCount(), 1);

    TestEvent testEvent;

    // PublishSync should immediately call the callback
    ep.PublishSync(testEvent);
    EXPECT_EQ(count, 1);
}
TEST(Event, Publish) {
    EventPublisher ep;
    EXPECT_EQ(ep.SubscriberCount(), 0);
    EXPECT_EQ(ep.EventCount(), 0);

    EventSubscriber es;
    EXPECT_EQ(es.SubscribedCount(), 0);

    int count = 0;
    es.SubscribeTo<TestEvent>(ep, [&count](const TestEvent& e) { ++count; });
    EXPECT_EQ(count, 0);
    EXPECT_EQ(es.SubscribedCount(), 1);
    EXPECT_EQ(ep.SubscriberCount(), 1);
    EXPECT_EQ(ep.EventCount(), 1);

    TestEvent testEvent;

    // Publish shouldn't trigger any callbacks. It should be processed in a later frame.
    ep.Publish(testEvent);
    EXPECT_EQ(count, 0);

    // All published events should trigger during Process.
    ep.Process();
    EXPECT_EQ(count, 1);

    // Processing a second time should do nothing since the queue was cleaned up.
    ep.Process();
    EXPECT_EQ(count, 1);

    ep.Publish(testEvent);
    ep.Publish(testEvent);
    EXPECT_EQ(count, 1);

    // Processing two of the same events should trigger the event twice.
    ep.Process();
    EXPECT_EQ(count, 3);
}
TEST(Event, CleanupSubscriber) {
    EventPublisher ep;
    EXPECT_EQ(ep.SubscriberCount(), 0);
    EXPECT_EQ(ep.EventCount(), 0);

    const TestEvent testEvent;
    int count = 0;

    {
        EventSubscriber es;
        EXPECT_EQ(es.SubscribedCount(), 0);

        es.SubscribeTo<TestEvent>(ep, [&count](const TestEvent& e) { ++count; });
        EXPECT_EQ(count, 0);
        EXPECT_EQ(es.SubscribedCount(), 1);
        EXPECT_EQ(ep.SubscriberCount(), 1);
        EXPECT_EQ(ep.EventCount(), 1);

        ep.PublishSync(testEvent);
        EXPECT_EQ(count, 1);
    }
    EXPECT_EQ(count, 1);
    EXPECT_EQ(ep.SubscriberCount(), 0);
    EXPECT_EQ(ep.EventCount(), 0);

    // Publishing an event shouldn't trigger a previously removed subscriber
    ep.PublishSync(testEvent);
    EXPECT_EQ(count, 1);
}

// Test that if an EventPublisher goes away before an EventSubscriber, the correct lists are updated to remove references to the deleted publisher.
TEST(Event, CleanupPublisher) {
    EventSubscriber es;
    EXPECT_EQ(es.SubscribedCount(), 0);

    const TestEvent testEvent;

    {
        EventPublisher ep;
        EXPECT_EQ(ep.SubscriberCount(), 0);
        EXPECT_EQ(ep.EventCount(), 0);

        int count = 0;
        es.SubscribeTo<TestEvent>(ep, [&count](const TestEvent& e) { ++count; });
        EXPECT_EQ(count, 0);
        EXPECT_EQ(es.SubscribedCount(), 1);
        EXPECT_EQ(ep.SubscriberCount(), 1);
        EXPECT_EQ(ep.EventCount(), 1);

        ep.PublishSync(testEvent);
        EXPECT_EQ(count, 1);
    }
    EXPECT_EQ(es.SubscribedCount(), 0);
}

// Test that if an EventPublisher goes away before an EventSubscriber, the correct lists are updated to remove references to the deleted publisher.
TEST(Event, DataIntegrity) {
    const DataEvent dataEvent(
        {"Test", "Test1", "Test2"},
        true,
        3.14
    );

    EventPublisher ep;
    EXPECT_EQ(ep.SubscriberCount(), 0);
    EXPECT_EQ(ep.EventCount(), 0);

    EventSubscriber es;
    EXPECT_EQ(es.SubscribedCount(), 0);

    bool eventTriggered = false;
    es.SubscribeTo<DataEvent>(ep, [&dataEvent, &eventTriggered](const DataEvent& e) 
    {
        EXPECT_EQ(dataEvent.m_Strings, e.m_Strings);
        EXPECT_EQ(dataEvent.m_Value, e.m_Value);
        EXPECT_EQ(dataEvent.m_Scale, e.m_Scale);
        eventTriggered = true;
    });
    EXPECT_EQ(es.SubscribedCount(), 1);
    EXPECT_EQ(ep.SubscriberCount(), 1);
    EXPECT_EQ(ep.EventCount(), 1);

    ep.PublishSync(dataEvent);
    EXPECT_TRUE(eventTriggered);

    eventTriggered = false;
    ep.Publish(dataEvent);
    ep.Process();
    EXPECT_TRUE(eventTriggered);
}