#pragma once

#include "../object/object.h"

class EventLoop;
class EventLoopGetterMixin
{
public:
    /**
     * @brief If getter is not null, get the event loop from getter.
     * If getter is null, get the event loop from current thread
     *
     * @param getter
     * @return ref<EventLoop>
     */
    static ref<EventLoop> ensureEventLoop(option<EventLoopGetterMixin> getter);
    virtual ref<EventLoop> eventLoop() = 0;
};

class EventLoop : public virtual Object, public EventLoopGetterMixin
{
public:
    /**
     * @brief The class mark the event loop has not yet complete
     * Tell the master event loop not to close yet when the event loop want to close
     * Call [Handle::dispose] let event loop to close when all handle disposed
     *
     */
    class Handle : public virtual Object
    {
        class _Handle;

    public:
        static ref<Handle> create(option<EventLoopGetterMixin> getter = nullptr);
        virtual void dispose() = 0;
    };

    static thread_local option<EventLoop> runningEventLoop;
    static thread_local bool isRunning;

    /**
     * @brief
     * Ensure a not yet run event loop or a running event loop
     * The closed event loop will be replace with a new not running event loop
     * You can access event loop object before it run via this api
     *
     * @return ref<EventLoop>
     */
    static ref<EventLoop> ensure() noexcept;

    /**
     * @brief
     * Schedular event loop on current thread
     *
     * @param fn The task will run just after the event scheduled
     * @return void
     */
    static void run(Function<void()> fn);

    /**
     * @brief Create a New [EventLoop] object and schedular it to a new thread
     *
     * @param fn The task will run just after the event scheduled
     * @return ref<EventLoop> the event running on the new thread
     */
    static ref<EventLoop> createEventLoopOnNewThread(Function<void()> fn);

    virtual void close() = 0;
    virtual bool alive() noexcept = 0;
    virtual void *nativeHandle() noexcept = 0;
    virtual void callSoon(Function<void()> fn) noexcept = 0;
    virtual void callSoonThreadSafe(Function<void()> fn) noexcept = 0;
    ref<EventLoop> eventLoop() override { return self(); }

protected:
    EventLoop() noexcept = default;
    class _EventLoop;
    friend class _ThreadEventLoop;
};
