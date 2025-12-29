#pragma once

#include <any>
#include <concepts>
#include <cstddef>
#include <functional>
#include <memory>
#include <mutex>
#include <typeindex>
#include <unordered_map>
#include <unordered_set>
#include <vector>

// Event system is heavily inspired by DeveloperPaul123's eventbus implementation:
// https://github.com/DeveloperPaul123/eventbus

namespace Airship {

class EventSubscriber;
class EventPublisher;

using EventCallbackFn = std::function<void(const std::any &)>;
struct EventCallback
{
    EventCallback(const EventCallbackFn &cb)
        : m_Handle(static_cast<const void *>(&cb)), m_Callback(cb) {}

    const void *m_Handle;
    EventCallbackFn m_Callback;
};

class EventWrapperInterface
{
public:
    virtual ~EventWrapperInterface() = default;

    virtual void PublishSync(EventPublisher *eventPublisher) = 0;
};

template <class EventType>
class EventWrapper;

class EventPublisher
{
public:
    virtual ~EventPublisher();

    template <class EventType, std::invocable<EventType> CallbackType>
    void AddSubscriber(EventSubscriber &subscriber, const CallbackType &callback)
    {
        auto it = m_EventCallbacks.emplace(
            typeid(EventType),
            [callback](std::any event)
            { callback(std::any_cast<EventType>(event)); });
        m_SubscriberCallbacks.emplace(&subscriber, it->second.m_Handle);
    }
    void RemoveSubscriber(EventSubscriber &subscriber);

    // Process all queued events and fire callbacks.
    void Process();

    // Queues an Event to be processed in the next frame.
    template <class EventType>
    void Publish(const EventType &event)
    {
        std::scoped_lock const lock(m_QueueMutex);
        m_QueuedEvents.push_back(std::make_unique<EventWrapper<EventType>>(event));
    }

    // Immediately fires event and handles resulting callbacks. Blocking function.
    template <class EventType>
    void PublishSync(const EventType &event)
    {
        auto eventsRange = m_EventCallbacks.equal_range(typeid(EventType));
        for (auto it = eventsRange.first; it != eventsRange.second; ++it)
        {
            it->second.m_Callback(std::any(event));
        }
    }

    [[nodiscard]] size_t EventCount() const { return m_EventCallbacks.size(); }
    [[nodiscard]] size_t SubscriberCount() const { return m_SubscriberCallbacks.size(); }

private:
    std::unordered_multimap<std::type_index, EventCallback> m_EventCallbacks;
    std::unordered_multimap<EventSubscriber *, const void *> m_SubscriberCallbacks;

    std::vector<std::unique_ptr<EventWrapperInterface>> m_QueuedEvents;
    std::mutex m_QueueMutex;
};

class EventSubscriber
{
public:
    virtual ~EventSubscriber();

    [[nodiscard]] size_t SubscribedCount() const { return m_Subscribed.size(); }

    template <class EventType, std::invocable<EventType> CallbackType>
    bool SubscribeTo(EventPublisher &pub, CallbackType &&callback)
    {
        m_Subscribed.insert(&pub);
        pub.AddSubscriber<EventType>(*this, std::forward<CallbackType>(callback));

        return true;
    }

    void CancelSubscription(EventPublisher &publisher);

private:
    std::unordered_set<EventPublisher *> m_Subscribed;
};

template <class EventType>
class EventWrapper : public EventWrapperInterface
{
public:
    EventWrapper(EventType e) : EventWrapperInterface(), m_Event(std::move(e)) {}

    void PublishSync(EventPublisher *eventPublisher) override
    {
        eventPublisher->PublishSync(m_Event);
    }

private:
    EventType m_Event;
};
} // namespace Airship
