#pragma once

#include <queue>
#include <list>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>

#include "../object.h"
#include "../basic/function.h"
#include "../basic/duration.h"
#include "../basic/state.h"

#include "state_helper.h"

// @ thread safe
template <typename T = std::nullptr_t>
class Future;
template <typename T = std::nullptr_t, typename std::enable_if<!std::is_void<T>::value>::type * = nullptr>
class Stream;
template <typename T = std::nullptr_t>
class Completer;

using Thread = std::thread;

// source from https://github.com/progschj/ThreadPool
class ThreadPool : public Object
{
public:
    ThreadPool(size_t);
    virtual ~ThreadPool();

    template <class F, class... Args>
    auto post(F &&f, Args &&... args) -> std::future<typename std::result_of<F(Args...)>::type>;
    template <class F, class... Args>
    auto microTask(F &&f, Args &&... args) -> std::future<typename std::result_of<F(Args...)>::type>;
    virtual bool isActive();
    virtual void dispose();
    virtual void forceClose();

protected:
    // need to keep track of threads so we can join them
    Object::List<Thread> workers;
    // the task queue
    std::list<Function<void()>> tasks;
    // the micro task queue
    std::list<Function<void()>> microTasks;

    // synchronization
    std::mutex queue_mutex;
    std::condition_variable condition;
    bool stop;
};

// add new work item to the pool
template <class F, class... Args>
auto ThreadPool::post(F &&f, Args &&... args) -> std::future<typename std::result_of<F(Args...)>::type>
{
    using return_type = typename std::result_of<F(Args...)>::type;

    auto task = std::make_shared<std::packaged_task<return_type()>>(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...));

    std::future<return_type> res = task->get_future();
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        assert(stop == false && "Don't allow posting after stopping the pool");
        tasks.emplace_back([task] { (*task)(); });
    }
    condition.notify_one();
    return res;
}

// add new work item to the pool
template <class F, class... Args>
auto ThreadPool::microTask(F &&f, Args &&... args) -> std::future<typename std::result_of<F(Args...)>::type>
{
    using return_type = typename std::result_of<F(Args...)>::type;

    auto task = std::make_shared<std::packaged_task<return_type()>>(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...));

    std::future<return_type> res = task->get_future();
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        assert(stop == false && "Don't allow posting after stopping the pool");
        microTasks.emplace_back([task] { (*task)(); });
    }
    condition.notify_one();
    return res;
}

class StaticThreadPool : public ThreadPool
{
public:
    static Object::Ref<ThreadPool> shared();
    StaticThreadPool(size_t threads = 1) : ThreadPool(threads) {}
    ~StaticThreadPool() { this->dispose(); }
};

template <>
class Future<std::nullptr_t> : public Object, public StateHelper
{
    template <typename R>
    friend class Completer;

protected:
    Future() {}
    // synchronization
    std::mutex _mutex;

public:
    virtual void sync() = 0;
    virtual void sync(Duration timeout) = 0;
};

template <>
class Future<void> : public Future<std::nullptr_t>
{
    template <typename R>
    friend class Completer;

public:
    static Object::Ref<Future<void>> value(Object::Ref<ThreadPool> callbackHandler);
    static Object::Ref<Future<void>> value(State<StatefulWidget> *state);

    static Object::Ref<Future<void>> async(Object::Ref<ThreadPool> callbackHandler, Function<void()>);
    static Object::Ref<Future<void>> async(State<StatefulWidget> *state, Function<void()>);

    Future(Object::Ref<ThreadPool> callbackHandler) : _callbackHandler(callbackHandler) {}
    Future(State<StatefulWidget> *state) : _callbackHandler(getHandlerfromState(state)) {}

    template <typename ReturnType = void>
    Object::Ref<Future<ReturnType>> than(Function<ReturnType()>);

    virtual Object::Ref<Future<void>> timeout(Duration, Function<void()> onTimeout);
    virtual Object::Ref<Future<void>> catchError(Function<void()>);
    void sync() override;
    void sync(Duration timeout) override;

protected:
    Object::Ref<Completer<void>> _completer;
    Object::Ref<ThreadPool> _callbackHandler;
    Object::List<Function<void()>> _callbackList;
};

template <typename T>
class Future : public Future<std::nullptr_t>
{
    template <typename R>
    friend class Completer;

public:
    static Object::Ref<Future<T>> value(Object::Ref<ThreadPool> callbackHandler, const T &);
    static Object::Ref<Future<T>> value(State<StatefulWidget> *state, const T &);

    static Object::Ref<Future<T>> async(Object::Ref<ThreadPool> callbackHandler, Function<T()>);
    static Object::Ref<Future<T>> async(State<StatefulWidget> *state, Function<T()>);

    Future(Object::Ref<ThreadPool> callbackHandler) : _callbackHandler(callbackHandler) {}
    Future(State<StatefulWidget> *state) : _callbackHandler(getHandlerfromState(state)) {}

    template <typename ReturnType, typename std::enable_if<!std::is_void<ReturnType>::value>::type * = nullptr>
    Object::Ref<Future<ReturnType>> than(Function<ReturnType(const T &)>);
    template <typename ReturnType = void, typename std::enable_if<std::is_void<ReturnType>::value>::type * = nullptr>
    Object::Ref<Future<ReturnType>> than(Function<ReturnType(const T &)> fn);

    virtual Object::Ref<Future<T>> timeout(Duration, Function<void()> onTimeout);
    virtual Object::Ref<Future<T>> catchError(Function<void()>);
    void sync() override;
    void sync(Duration timeout) override;

protected:
    Object::Ref<Completer<T>> _completer;
    Object::Ref<ThreadPool> _callbackHandler;
    Object::List<Function<void(const T &)>> _callbackList;
};

template <>
class Completer<std::nullptr_t> : public Object, public StateHelper
{
    template <typename R>
    friend class Future;

protected:
    Completer() {}

public:
    virtual bool cancel() = 0;
    virtual bool isCompleted() = 0;
    virtual bool isCancelled() = 0;
};

template <>
class Completer<void> : public Completer<std::nullptr_t>
{
    template <typename R>
    friend class Future;

public:
    static Object::Ref<Completer<void>> deferred(Object::Ref<ThreadPool> callbackHandler, Function<void()> fn);
    static Object::Ref<Completer<void>> deferred(State<StatefulWidget> *state, Function<void()> fn);

    Completer(Object::Ref<ThreadPool> callbackHandler) : _callbackHandler(callbackHandler), _future(Object::create<Future<void>>(callbackHandler)) {}
    Completer(State<StatefulWidget> *state) : _callbackHandler(getHandlerfromState(state)), _future(Object::create<Future<void>>(state)) {}

    virtual void complete()
    {
        assert(this->_isCompleted == false);
        std::unique_lock<std::mutex> lock(this->_future->_mutex);
        if (this->_isCancelled)
            return;
        this->_future->_completer = Object::cast<>(this);
        for (auto &fn : this->_future->_callbackList)
            this->_callbackHandler->post(fn);
        this->_future->_callbackList.clear();
        this->_future = nullptr;
    }

    bool cancel() override
    {
        std::unique_lock<std::mutex> lock(this->_future->_mutex);
        if (this->_isCompleted)
            return false; // already completed
        this->_isCancelled = true;
        return true; // cancel successfully
    }

    virtual Object::Ref<Future<void>> getFuture() const
    {
        return this->_future;
    }

    bool isCompleted() override
    {
        std::unique_lock<std::mutex> lock(this->_future->_mutex);
        return this->_isCompleted;
    }

    bool isCancelled() override
    {
        std::unique_lock<std::mutex> lock(this->_future->_mutex);
        return this->_isCancelled;
    }

protected:
    bool _isCompleted = false;
    bool _isCancelled = false;
    Object::Ref<Future<void>> _future;
    Object::Ref<ThreadPool> _callbackHandler;
};

template <typename T>
class Completer : public Completer<std::nullptr_t>
{
    template <typename R>
    friend class Future;

public:
    static Object::Ref<Completer<T>> deferred(Object::Ref<ThreadPool> callbackHandler, Function<T()> fn);
    static Object::Ref<Completer<T>> deferred(State<StatefulWidget> *state, Function<T()> fn);

    Completer(Object::Ref<ThreadPool> callbackHandler) : _callbackHandler(callbackHandler), _future(Object::create<Future<T>>(callbackHandler)) {}
    Completer(State<StatefulWidget> *state) : _callbackHandler(getHandlerfromState(state)), _future(Object::create<Future<T>>(state)) {}

    virtual void complete(const T &value)
    {
        assert(this->_isCompleted == false);
        Object::Ref<Completer<T>> self = Object::cast<>(this);
        std::unique_lock<std::mutex> lock(this->_future->_mutex);
        if (this->_isCancelled)
            return;
        this->_future->_completer = Object::cast<>(this);
        this->_data = value;
        for (auto &fn : this->_future->_callbackList)
            this->_callbackHandler->post([fn, self] { fn(self->_data); });
        this->_future->_callbackList.clear();
        this->_future = nullptr;
    }

    virtual void complete(T &&value)
    {
        assert(this->_isCompleted == false);
        Object::Ref<Completer<T>> self = Object::cast<>(this);
        std::unique_lock<std::mutex> lock(this->_future->_mutex);
        if (this->_isCancelled)
            return;
        this->_future->_completer = Object::cast<>(this);
        this->_data = std::forward<T>(value);
        for (auto &fn : this->_future->_callbackList)
            this->_callbackHandler->post([fn, self] { fn(self->_data); });
        this->_future->_callbackList.clear();
        this->_future = nullptr;
    }

    bool cancel() override
    {
        std::unique_lock<std::mutex> lock(this->_future->_mutex);
        if (this->_isCompleted)
            return false; // already completed
        this->_isCancelled = true;
        return true; // cancel successfully
    }

    virtual Object::Ref<Future<T>> getFuture() const
    {
        return this->_future;
    }

    bool isCompleted() override
    {
        std::unique_lock<std::mutex> lock(this->_future->_mutex);
        return this->_isCompleted;
    }

    bool isCancelled() override
    {
        std::unique_lock<std::mutex> lock(this->_future->_mutex);
        return this->_isCancelled;
    }

protected:
    T _data;
    bool _isCompleted = false;
    bool _isCancelled = false;
    Object::Ref<Future<T>> _future;
    Object::Ref<ThreadPool> _callbackHandler;
};

inline Object::Ref<Future<void>> Future<void>::value(Object::Ref<ThreadPool> callbackHandler)
{
    Object::Ref<Completer<void>> completer = Object::create<Completer<void>>(callbackHandler);
    completer->complete();
    return completer->getFuture();
}

inline Object::Ref<Future<void>> Future<void>::value(State<StatefulWidget> *state)
{
    Object::Ref<Completer<void>> completer = Object::create<Completer<void>>(state);
    completer->complete();
    return completer->getFuture();
}

template <typename T>
Object::Ref<Future<T>> Future<T>::value(Object::Ref<ThreadPool> callbackHandler, const T &value)
{
    Object::Ref<Completer<T>> completer = Object::create<Completer<T>>(callbackHandler);
    completer->complete(value);
    return completer->getFuture();
}

template <typename T>
Object::Ref<Future<T>> Future<T>::value(State<StatefulWidget> *state, const T &value)
{
    Object::Ref<Completer<T>> completer = Object::create<Completer<T>>(state);
    completer->complete(value);
    return completer->getFuture();
}

inline Object::Ref<Future<void>> Future<void>::async(Object::Ref<ThreadPool> callbackHandler, Function<void()> fn)
{
    Object::Ref<Completer<void>> completer = Completer<void>::deferred(callbackHandler, fn);
    return completer->getFuture();
}

inline Object::Ref<Future<void>> Future<void>::async(State<StatefulWidget> *state, Function<void()> fn)
{
    Object::Ref<Completer<void>> completer = Completer<void>::deferred(state, fn);
    return completer->getFuture();
}

template <typename T>
Object::Ref<Future<T>> Future<T>::async(Object::Ref<ThreadPool> callbackHandler, Function<T()> fn)
{
    Object::Ref<Completer<T>> completer = Completer<T>::deferred(callbackHandler, fn);
    return completer->getFuture();
}

template <typename T>
Object::Ref<Future<T>> Future<T>::async(State<StatefulWidget> *state, Function<T()> fn)
{
    Object::Ref<Completer<T>> completer = Completer<T>::deferred(state, fn);
    return completer->getFuture();
}

template <>
inline Object::Ref<Future<void>> Future<void>::than(Function<void()> fn)
{
    std::unique_lock<std::mutex> lock(this->_mutex);
    Object::Ref<Completer<void>> completer = Object::create<Completer<void>>(this->_callbackHandler);
    if (this->_completer == nullptr)
        this->_callbackList.push_back([completer, fn] { fn(); completer->complete(); });
    else
        this->_callbackHandler->post([completer, fn] { fn(); completer->complete(); });
    return completer->getFuture();
}

template <typename ReturnType>
Object::Ref<Future<ReturnType>> Future<void>::than(Function<ReturnType()> fn)
{
    std::unique_lock<std::mutex> lock(this->_mutex);
    Object::Ref<Completer<ReturnType>> completer = Object::create<Completer<ReturnType>>(this->_callbackHandler);
    if (this->_completer == nullptr)
        this->_callbackList.push_back([completer, fn] { completer->complete(fn()); });
    else
        this->_callbackHandler->post([completer, fn] { completer->complete(fn()); });
    return completer->getFuture();
}

inline Object::Ref<Future<void>> Future<void>::timeout(Duration, Function<void()> onTimeout)
{
    return Object::cast<>(this);
}

inline Object::Ref<Future<void>> Future<void>::catchError(Function<void()>)
{
    return Object::cast<>(this);
}

inline void Future<void>::sync()
{
    std::mutex mutex;
    std::condition_variable condition;
    std::unique_lock<std::mutex> lock(mutex);
    this->than<void>([&] { condition.notify_all(); });
    condition.wait(lock);
}

inline void Future<void>::sync(Duration timeout)
{
    std::mutex mutex;
    std::condition_variable condition;
    std::unique_lock<std::mutex> lock(mutex);
    this->than<void>([&] { condition.notify_all(); });
    condition.wait(lock);
}

template <typename T>
template <typename ReturnType, typename std::enable_if<std::is_void<ReturnType>::value>::type *>
Object::Ref<Future<ReturnType>> Future<T>::than(Function<ReturnType(const T &)> fn)
{
    std::unique_lock<std::mutex> lock(this->_mutex);
    Object::Ref<Completer<ReturnType>> completer = Object::create<Completer<ReturnType>>(this->_callbackHandler);
    Object::Ref<Future<T>> self = Object::cast<>(this);
    if (this->_completer == nullptr)
        this->_callbackList.push_back([completer, fn](const T &value) { fn(value); completer->complete(); });
    else
        this->_callbackHandler->post([completer, self, fn] { fn(self->_completer->_data); completer->complete(); });
    return completer->getFuture();
}

template <typename T>
template <typename ReturnType, typename std::enable_if<!std::is_void<ReturnType>::value>::type *>
Object::Ref<Future<ReturnType>> Future<T>::than(Function<ReturnType(const T &)> fn)
{
    std::unique_lock<std::mutex> lock(this->_mutex);
    Object::Ref<Completer<ReturnType>> completer = Object::create<Completer<ReturnType>>(this->_callbackHandler);
    Object::Ref<Future<T>> self = Object::cast<>(this);
    if (this->_completer == nullptr)
        this->_callbackList.push_back([completer, fn](const T &value) { completer->complete(fn(value)); });
    else
        this->_callbackHandler->post([completer, self, fn] { completer->complete(fn(self->_completer->_data)); });
    return completer->getFuture();
}

template <typename T>
Object::Ref<Future<T>> Future<T>::timeout(Duration, Function<void()> onTimeout)
{
    return Object::cast<>(this);
}

template <typename T>
Object::Ref<Future<T>> Future<T>::catchError(Function<void()>)
{
    return Object::cast<>(this);
}

template <typename T>
void Future<T>::sync()
{
    std::mutex mutex;
    std::condition_variable condition;
    std::unique_lock<std::mutex> lock(mutex);
    this->than<void>([&](const T &) { condition.notify_all(); });
    condition.wait(lock);
}

template <typename T>
void Future<T>::sync(Duration timeout)
{
    std::mutex mutex;
    std::condition_variable condition;
    std::unique_lock<std::mutex> lock(mutex);
    this->than<void>([&](const T &) -> void {
        condition.notify_all();
    });
    condition.wait(lock);
}

template <>
class Stream<std::nullptr_t> : public Object, public StateHelper
{
protected:
    Stream() {}
    virtual void close() = 0;
};

template <typename T, typename std::enable_if<!std::is_void<T>::value>::type *>
class Stream : public Stream<std::nullptr_t>
{
public:
    Stream(Object::Ref<ThreadPool> callbackHandler) : _callbackHandler(callbackHandler), _onClose(Object::create<Completer<void>>(callbackHandler)) {}
    Stream(State<StatefulWidget> *state) : _callbackHandler(getHandlerfromState(state)), _onClose(Object::create<Completer<void>>(state)) {}
    virtual ~Stream()
    {
        if (!this->_isClosed)
            this->close();
    }

    virtual Object::Ref<Stream<T>> sink(const T &value)
    {
        std::unique_lock<std::mutex> lock(this->_mutex);
        assert(!this->_isClosed);
        Object::Ref<Stream<T>> self = Object::cast<>(this);
        if (this->_listener)
            this->_callbackHandler->post([self, value] { self->_listener(std::move(value)); });
        else
            this->_cache.emplace_back(value);
        return self;
    }

    virtual Object::Ref<Stream<T>> sink(T &&value)
    {
        std::unique_lock<std::mutex> lock(this->_mutex);
        assert(!this->_isClosed);
        Object::Ref<Stream<T>> self = Object::cast<>(this);
        if (this->_listener)
            this->_callbackHandler->post([self, value] { self->_listener(std::move(value)); });
        else
            this->_cache.emplace_back(std::forward<T>(value));
        return self;
    }

    virtual Object::Ref<Stream<T>> listen(Function<void(T)> fn)
    {
        std::unique_lock<std::mutex> lock(this->_mutex);
        this->_listener = fn;
        Object::Ref<Stream<T>> self = Object::cast<>(this);
        for (auto &cache : this->_cache)
            this->_callbackHandler->post([self, cache] { self->_listener(std::move(cache)); });
        this->_cache.clear();
        if (this->_isClosed)
            this->_onClose->complete();
        return self;
    }

    virtual Object::Ref<Stream<T>> onClose(Function<void()> fn)
    {
        std::unique_lock<std::mutex> lock(this->_mutex);
        this->_onClose->getFuture()->than(fn);
        return Object::cast<>(this);
    }

    Future<void> asFuture()
    {
        std::unique_lock<std::mutex> lock(this->_mutex);
        assert(!this->_isClosed);
        return this->_onClose->getFuture();
    }

    void close() override
    {
        std::unique_lock<std::mutex> lock(this->_mutex);
        assert(!this->_isClosed);
        if (this->_cache.empty())
            this->_onClose->complete();
        this->_isClosed = true;
    }

protected:
    Object::Ref<Completer<void>> _onClose;
    Object::Ref<ThreadPool> _callbackHandler;
    Object::List<T> _cache;

    Function<void(T)> _listener;
    std::mutex _mutex;
    bool _isClosed;
};

inline Object::Ref<Completer<void>> Completer<void>::deferred(Object::Ref<ThreadPool> callbackHandler, Function<void()> fn)
{
    Object::Ref<Completer<void>> completer = Object::create<Completer<void>>(callbackHandler);
    completer->_callbackHandler->post([fn, completer] {fn(); completer->complete(); });
    return completer;
}

inline Object::Ref<Completer<void>> Completer<void>::deferred(State<StatefulWidget> *state, Function<void()> fn)
{
    Object::Ref<Completer<void>> completer = Object::create<Completer<void>>(state);
    completer->_callbackHandler->post([fn, completer] {fn(); completer->complete(); });
    return completer;
}
template <typename T>
Object::Ref<Completer<T>> Completer<T>::deferred(Object::Ref<ThreadPool> callbackHandler, Function<T()> fn)
{
    Object::Ref<Completer<T>> completer = Object::create<Completer<T>>(callbackHandler);
    completer->_callbackHandler->post([fn, completer] { completer->complete(fn()); });
    return completer;
}

template <typename T>
Object::Ref<Completer<T>> Completer<T>::deferred(State<StatefulWidget> *state, Function<T()> fn)
{
    Object::Ref<Completer<T>> completer = Object::create<Completer<T>>(state);
    completer->_callbackHandler->post([fn, completer] { completer->complete(fn()); });
    return completer;
}