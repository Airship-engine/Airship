#include "core/event.h"

#include <iostream>

namespace Airship {

EventPublisher::~EventPublisher()
{
    for(auto& [subscriber, callbacks] : m_SubscriberCallbacks )
        subscriber->CancelSubscription(*this); 
}

void EventPublisher::RemoveSubscriber(EventSubscriber& subscriber)
{
    auto subscriberCallbacksRange = m_SubscriberCallbacks.equal_range(&subscriber);
    for (auto it=subscriberCallbacksRange.first; it != subscriberCallbacksRange.second; ++it)
    {
        std::erase_if(m_EventCallbacks, [&it](const auto& item) {
            return item.second.m_Handle == it->second;
        });
    }

    m_SubscriberCallbacks.erase(&subscriber);
}

void EventPublisher::Process() 
{
    std::unique_lock<std::mutex> lock(m_QueueMutex);
    std::vector<std::unique_ptr<EventWrapperInterface>> events;
    std::swap(events, m_QueuedEvents);
    lock.unlock();

    for (const auto& eventWrapper : events)
        eventWrapper->PublishSync(this);
}

void EventSubscriber::CancelSubscription(EventPublisher& publisher)
{
    m_Subscribed.erase(&publisher);
}

EventSubscriber::~EventSubscriber()
{
    for (auto publisher : m_Subscribed)
        publisher->RemoveSubscriber(*this);
}
} // namespace Airship
