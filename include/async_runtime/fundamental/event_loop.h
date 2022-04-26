#pragma once

#include "../object.h"

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

class EventLoop : public Object, public EventLoopGetterMixin
{
public:
    /**
     * @brief The class mark the event has not yet complete
     * Tell the master event loop not to close yet when the event loop want to close
     * Call [Handle::dispose] let event loop to close when all handle disposed
     *
     */
    class Handle : virtual public Object
    {
        class _Handle;

    public:
        static ref<Handle> create(option<EventLoopGetterMixin> getter = nullptr);
        virtual void dispose() = 0;
        virtual ~Handle() {}
    };

    class WithHandleMixin
    {
    protected:
        ref<Handle> handle;
        WithHandleMixin(option<EventLoopGetterMixin> getter = nullptr) : handle(Handle::create(getter)) {}
    };

    static thread_local option<EventLoop> runningEventLoop;
    static thread_local bool isRunning;

    /**
     * @brief
     * Ensure a not yet run event loop or a running event loop
     * The closed event loop will be replace with a new not running event loop
     * Please call this before [EventLoop::run]
     *
     * @return ref<EventLoop>
     */
    static ref<EventLoop> ensure() noexcept;

    /**
     * @brief
     * Schedular event loop on current thread
     * Please call after [EventLoop::ensure]
     *
     * @param fn The task will run just after the event scheduled
     * @return int The return code from [uv_run]
     */
    static int run(Function<void()> fn);

    /**
     * @brief Create a New [EventLoop] object and schedular it to a new thread
     *
     * @param fn The task will run just after the event scheduled
     * @return ref<EventLoop>
     */
    static ref<EventLoop> createEventLoopOnNewThread(Function<void()> fn);

    virtual void close() = 0;
    virtual bool alive() const noexcept = 0;
    virtual void *nativeHandle() noexcept = 0;
    virtual void callSoon(Function<void()> fn) = 0;
    ref<EventLoop> eventLoop() override { return self(); }

protected:
    EventLoop() noexcept {}
    class _EventLoop;
    friend class _ThreadEventLoop;
};
