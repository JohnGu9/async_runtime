#pragma once

#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>

#include "../object.h"

// @ thread safe
template <typename T = std::nullptr_t>
class Future;
template <typename T = std::nullptr_t>
class Stream;
template <typename T = std::nullptr_t>
class Completer;
template <typename T = std::nullptr_t>
class AsyncSnapshot;

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
    std::vector<Thread> workers;
    // the task queue
    std::queue<std::function<void()>> tasks;
    // the micro task queue
    std::queue<std::function<void()>> microTasks;

    // synchronization
    std::mutex queue_mutex;
    std::condition_variable condition;
    bool stop;
};

static bool warning()
{
    std::cout << "Async task post after threadpool stopped. " << std::endl;
    return true;
}

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
        if (stop)
        {
            assert(warning());
            (*task)();
            return res;
        }
        tasks.emplace([task] { (*task)(); });
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
        if (stop)
        {
            assert(warning());
            (*task)();
            return res;
        }
        microTasks.emplace([task] { (*task)(); });
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

#include "../basic/function.h"
#include "../basic/duration.h"
#include "../widgets/state.h"
#include "state_helper.h"

template <>
class Future<std::nullptr_t> : public Object, StateHelper
{
    template <typename R>
    friend class Completer;

    template <typename R>
    friend class Stream;

protected:
    Future(Object::Ref<ThreadPool> callbackHandler) : _callbackHandler(callbackHandler), _completed(false) {}
    Future(State<StatefulWidget> *state) : _callbackHandler(getHandlerfromState(state)), _completed(false) {}
    std::atomic_bool _completed;
    Object::Ref<ThreadPool> _callbackHandler;

public:
    static Object::Ref<Future<void>> race(Set<Object::Ref<Future<>>> &&set);
    static Object::Ref<Future<void>> wait(Set<Object::Ref<Future<>>> &&set);

    virtual Object::Ref<Future<void>> than(Function<void()>) = 0;
    virtual void sync() = 0;
    virtual void sync(Duration timeout) = 0;
};

template <>
class Future<void> : public Future<std::nullptr_t>
{
    template <typename R>
    friend class Completer;

    template <typename R>
    friend class Stream;

public:
    static Object::Ref<Future<void>> value(Object::Ref<ThreadPool> callbackHandler);
    static Object::Ref<Future<void>> value(State<StatefulWidget> *state);

    static Object::Ref<Future<void>> async(Object::Ref<ThreadPool> callbackHandler, Function<void()>);
    static Object::Ref<Future<void>> async(State<StatefulWidget> *state, Function<void()>);

    Future(Object::Ref<ThreadPool> callbackHandler) : Future<std::nullptr_t>(callbackHandler) {}
    Future(State<StatefulWidget> *state) : Future<std::nullptr_t>(state) {}

    template <typename ReturnType = void>
    Object::Ref<Future<ReturnType>> than(Function<ReturnType()>);
    Object::Ref<Future<void>> than(Function<void()>) override;

    virtual Object::Ref<Future<void>> timeout(Duration, Function<void()> onTimeout);
    void sync() override;
    void sync(Duration timeout) override;

protected:
    List<Function<void()>> _callbackList;
};

template <typename T>
class Future : public Future<std::nullptr_t>
{
    template <typename R>
    friend class Completer;

    template <typename R>
    friend class Stream;

public:
    static Object::Ref<Future<T>> value(Object::Ref<ThreadPool> callbackHandler, const T &);
    static Object::Ref<Future<T>> value(State<StatefulWidget> *state, const T &);
    static Object::Ref<Future<T>> value(Object::Ref<ThreadPool> callbackHandler, T &&);
    static Object::Ref<Future<T>> value(State<StatefulWidget> *state, T &&);

    static Object::Ref<Future<T>> async(Object::Ref<ThreadPool> callbackHandler, Function<T()>);
    static Object::Ref<Future<T>> async(State<StatefulWidget> *state, Function<T()>);

    Future(Object::Ref<ThreadPool> callbackHandler) : Future<std::nullptr_t>(callbackHandler) {}
    Future(State<StatefulWidget> *state) : Future<std::nullptr_t>(state) {}

    Future(Object::Ref<ThreadPool> callbackHandler, const T &data)
        : _data(data), Future<std::nullptr_t>(callbackHandler) { this->_completed = true; }
    Future(State<StatefulWidget> *state, const T &data)
        : _data(data), Future<std::nullptr_t>(state) { this->_completed = true; }

    template <typename ReturnType, typename std::enable_if<!std::is_void<ReturnType>::value>::type * = nullptr>
    Object::Ref<Future<ReturnType>> than(Function<ReturnType(const T &)>);
    template <typename ReturnType = void, typename std::enable_if<std::is_void<ReturnType>::value>::type * = nullptr>
    Object::Ref<Future<ReturnType>> than(Function<ReturnType(const T &)> fn);
    Object::Ref<Future<void>> than(Function<void()>) override;

    virtual Object::Ref<Future<T>> timeout(Duration, Function<void()> onTimeout);
    void sync() override;
    void sync(Duration timeout) override;

protected:
    T _data;
    List<Function<void(const T &)>> _callbackList;
};

template <>
class Completer<std::nullptr_t> : public Object, StateHelper
{
    template <typename R>
    friend class Future;

    template <typename R>
    friend class Stream;

protected:
    Completer(Object::Ref<ThreadPool> callbackHandler) : _callbackHandler(callbackHandler), _isCompleted(false), _isCancelled(false) {}
    Completer(State<StatefulWidget> *state) : _callbackHandler(getHandlerfromState(state)), _isCompleted(false), _isCancelled(false) {}

    std::atomic_bool _isCompleted;
    std::atomic_bool _isCancelled;
    Object::Ref<ThreadPool> _callbackHandler;

public:
    virtual void cancel() = 0;
    const std::atomic_bool &isCompleted = _isCompleted;
    const std::atomic_bool &isCancelled = _isCancelled;
};

template <>
class Completer<void> : public Completer<std::nullptr_t>
{
    template <typename R>
    friend class Future;

    template <typename R>
    friend class Stream;

public:
    static Object::Ref<Completer<void>> deferred(Object::Ref<ThreadPool> callbackHandler, Function<void()> fn);
    static Object::Ref<Completer<void>> deferred(State<StatefulWidget> *state, Function<void()> fn);

    Completer(Object::Ref<ThreadPool> callbackHandler) : Completer<std::nullptr_t>(callbackHandler), _future(Object::create<Future<void>>(callbackHandler)) {}
    Completer(State<StatefulWidget> *state) : Completer<std::nullptr_t>(state), _future(Object::create<Future<void>>(state)) {}

    virtual void complete()
    {
        Object::Ref<Completer<void>> self = Object::cast<>(this);
        this->_callbackHandler->post([self] {
            assert(self->_isCompleted == false);
            if (self->_isCancelled)
                return;
            self->_future->_completed = true;
            for (auto &fn : self->_future->_callbackList)
                fn();
            self->_future->_callbackList.clear();
        });
    }

    void cancel() override
    {
        Object::Ref<Completer<void>> self = Object::cast<>(this);
        this->_callbackHandler->post([self] {
            if (self->_isCompleted)
                return false; // already completed
            self->_isCancelled = true;
            return true; // cancel successfully
        });
    }

protected:
    Object::Ref<Future<void>> _future;

public:
    const Object::Ref<Future<void>> &future = _future;
};

template <typename T>
class Completer : public Completer<std::nullptr_t>
{
    template <typename R>
    friend class Future;

    template <typename R>
    friend class Stream;

public:
    static Object::Ref<Completer<T>> deferred(Object::Ref<ThreadPool> callbackHandler, Function<T()> fn);
    static Object::Ref<Completer<T>> deferred(State<StatefulWidget> *state, Function<T()> fn);

    Completer(Object::Ref<ThreadPool> callbackHandler) : Completer<std::nullptr_t>(callbackHandler), _future(Object::create<Future<T>>(callbackHandler)) {}
    Completer(State<StatefulWidget> *state) : Completer<std::nullptr_t>(state), _future(Object::create<Future<T>>(state)) {}

    virtual void complete(const T &value)
    {
        Object::Ref<Completer<T>> self = Object::cast<>(this);
        this->_callbackHandler->post([self, value] {
            assert(self->_isCompleted == false);
            if (self->_isCancelled)
                return;
            self->_future->_data = std::move(value);
            self->_future->_completed = true;
            for (auto &fn : self->_future->_callbackList)
                fn(self->_future->_data);
            self->_future->_callbackList.clear();
            self->_future = nullptr;
        });
    }

    virtual void complete(T &&value)
    {
        Object::Ref<Completer<T>> self = Object::cast<>(this);
        this->_callbackHandler->post([self, value] {
            assert(self->_isCompleted == false);
            if (self->_isCancelled)
                return;
            self->_future->_data = std::move(value);
            self->_future->_completed = true;
            for (auto &fn : self->_future->_callbackList)
                fn(self->_future->_data);
            self->_future->_callbackList.clear();
        });
    }

    void cancel() override
    {
        Object::Ref<Completer<T>> self = Object::cast<>(this);
        this->_callbackHandler->post([self] {
            if (!self->_isCompleted)
                self->_isCancelled = true;
        });
    }

protected:
    Object::Ref<Future<T>> _future;

public:
    const Object::Ref<Future<T>> &future = _future;
};

inline Object::Ref<Future<void>> Future<std::nullptr_t>::race(Set<Object::Ref<Future<>>> &&set)
{
    //TODO: implement function
    assert(!set.empty());
    Object::Ref<Completer<void>> completer = Object::create<Completer<void>>((*set.begin())->_callbackHandler);
    return completer->future;
}

inline Object::Ref<Future<void>> Future<std::nullptr_t>::wait(Set<Object::Ref<Future<>>> &&set)
{
    //TODO: implement function
    assert(!set.empty());
    Object::Ref<Completer<void>> completer = Object::create<Completer<void>>((*set.begin())->_callbackHandler);
    return completer->future;
}

inline Object::Ref<Future<void>> Future<void>::value(Object::Ref<ThreadPool> callbackHandler)
{
    //TODO: implement function
    Object::Ref<Completer<void>> completer = Object::create<Completer<void>>(callbackHandler);
    completer->complete();
    return completer->future;
}

inline Object::Ref<Future<void>> Future<void>::value(State<StatefulWidget> *state)
{
    //TODO: implement function
    Object::Ref<Completer<void>> completer = Object::create<Completer<void>>(state);
    completer->complete();
    return completer->future;
}

template <typename T>
Object::Ref<Future<T>> Future<T>::value(Object::Ref<ThreadPool> callbackHandler, const T &value)
{
    return Object::create<Future<T>>(callbackHandler, value);
}

template <typename T>
Object::Ref<Future<T>> Future<T>::value(State<StatefulWidget> *state, const T &value)
{
    return Object::create<Future<T>>(state, value);
}

template <typename T>
Object::Ref<Future<T>> Future<T>::value(Object::Ref<ThreadPool> callbackHandler, T &&value)
{
    return Object::create<Future<T>>(callbackHandler, value);
}

template <typename T>
Object::Ref<Future<T>> Future<T>::value(State<StatefulWidget> *state, T &&value)
{
    return Object::create<Future<T>>(state, value);
}

inline Object::Ref<Future<void>> Future<void>::async(Object::Ref<ThreadPool> callbackHandler, Function<void()> fn)
{
    Object::Ref<Completer<void>> completer = Completer<void>::deferred(callbackHandler, fn);
    return completer->future;
}

inline Object::Ref<Future<void>> Future<void>::async(State<StatefulWidget> *state, Function<void()> fn)
{
    Object::Ref<Completer<void>> completer = Completer<void>::deferred(state, fn);
    return completer->future;
}

template <typename T>
Object::Ref<Future<T>> Future<T>::async(Object::Ref<ThreadPool> callbackHandler, Function<T()> fn)
{
    Object::Ref<Completer<T>> completer = Completer<T>::deferred(callbackHandler, fn);
    return completer->future;
}

template <typename T>
Object::Ref<Future<T>> Future<T>::async(State<StatefulWidget> *state, Function<T()> fn)
{
    Object::Ref<Completer<T>> completer = Completer<T>::deferred(state, fn);
    return completer->future;
}

template <>
inline Object::Ref<Future<void>> Future<void>::than(Function<void()> fn)
{
    Object::Ref<Future<void>> self = Object::cast<>(this);
    Object::Ref<Completer<void>> completer = Object::create<Completer<void>>(this->_callbackHandler);
    this->_callbackHandler->post([self, completer, fn] {
        if (self->_completed == false)
            self->_callbackList.push_back([completer, fn] { fn(); completer->complete(); });
        else
        {
            fn();
            completer->complete();
        }
    });
    return completer->future;
}

template <typename ReturnType>
Object::Ref<Future<ReturnType>> Future<void>::than(Function<ReturnType()> fn)
{
    Object::Ref<Future<void>> self = Object::cast<>(this);
    Object::Ref<Completer<ReturnType>> completer = Object::create<Completer<ReturnType>>(this->_callbackHandler);
    this->_callbackHandler->post([self, completer, fn] {
        if (self->_completed == false)
            self->_callbackList.push_back([completer, fn] { completer->complete(fn()); });
        else
            completer->complete(fn());
    });
    return completer->future;
}

inline Object::Ref<Future<void>> Future<void>::than(Function<void()> fn)
{
    Object::Ref<Future<void>> self = Object::cast<>(this);
    this->_callbackHandler->post([self, fn] {
        if (self->_completed == false)
            self->_callbackList.push_back(fn);
        else
            fn();
    });

    return self;
}

inline Object::Ref<Future<void>> Future<void>::timeout(Duration, Function<void()> onTimeout)
{
    //TODO: implement function
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
    Object::Ref<Completer<void>> completer = Object::create<Completer<void>>(this->_callbackHandler);
    Object::Ref<Future<T>> self = Object::cast<>(this);
    this->_callbackHandler->post([self, completer, fn] {
        if (self->_completed == false)
            self->_callbackList.push_back([completer, fn](const T &value) { fn(value); completer->complete(); });
        else
        {
            fn(self->_data);
            completer->complete();
        }
    });
    return completer->future;
}

template <typename T>
template <typename ReturnType, typename std::enable_if<!std::is_void<ReturnType>::value>::type *>
Object::Ref<Future<ReturnType>> Future<T>::than(Function<ReturnType(const T &)> fn)
{
    Object::Ref<Completer<ReturnType>> completer = Object::create<Completer<ReturnType>>(this->_callbackHandler);
    Object::Ref<Future<T>> self = Object::cast<>(this);
    this->_callbackHandler->post([self, completer, fn] {
        if (self->_completed == false)
            self->_callbackList.push_back([completer, fn](const T &value) { completer->complete(fn(value)); });
        else
            completer->complete(fn(self->_data));
    });
    return completer->future;
}

template <typename T>
Object::Ref<Future<void>> Future<T>::than(Function<void()> fn)
{
    return this->than<void>([fn](const T &) { fn(); });
}

template <typename T>
Object::Ref<Future<T>> Future<T>::timeout(Duration, Function<void()> onTimeout)
{
    //TODO: implement function
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
    this->than<void>([&](const T &) { condition.notify_all(); });
    condition.wait(lock);
}

template <>
class Stream<std::nullptr_t> : public Object, StateHelper
{
protected:
    Stream(Object::Ref<ThreadPool> callbackHandler) : _callbackHandler(callbackHandler) {}
    Stream(State<StatefulWidget> *state) : _callbackHandler(getHandlerfromState(state)) {}

    Object::Ref<ThreadPool> _callbackHandler;

public:
    virtual void close() = 0;
};

template <>
class Stream<void> : public Stream<std::nullptr_t>
{
public:
    Stream(Object::Ref<ThreadPool> callbackHandler) : Stream<std::nullptr_t>(callbackHandler), _onClose(Object::create<Completer<void>>(callbackHandler)) {}
    Stream(State<StatefulWidget> *state) : Stream<std::nullptr_t>(state), _onClose(Object::create<Completer<void>>(state)) {}
    virtual ~Stream()
    {
        if (!this->_isClosed)
            this->_onClose->complete();
    }

    virtual Object::Ref<Stream<void>> sink()
    {
        Object::Ref<Stream<void>> self = Object::cast<>(this);
        this->_callbackHandler->post([self] {
            assert(!self->_isClosed);
            if (self->_listener)
                self->_listener();
            else
                self->_sinkCounter++;
        });
        return self;
    }

    virtual Object::Ref<Stream<void>> listen(Function<void()> fn)
    {
        Object::Ref<Stream<void>> self = Object::cast<>(this);
        this->_callbackHandler->post([self, fn] {
            assert(!static_cast<bool>(self->_listener) && "Single listener stream can't have more than one listener");
            self->_listener = fn;
            for (int c = 0; c < self->_sinkCounter; c++)
                self->_listener();
            self->_sinkCounter = 0;
            if (self->_isClosed)
                self->_onClose->complete();
        });
        return self;
    }

    virtual Object::Ref<Stream<void>> onClose(Function<void()> fn)
    {
        this->_onClose->future->than(fn);
        return Object::cast<>(this);
    }

    virtual Object::Ref<Future<void>> asFuture()
    {
        return this->_onClose->future;
    }

    void close() override
    {
        Object::Ref<Stream<void>> self = Object::cast<>(this);
        this->_callbackHandler->post([self] {
            assert(!self->_isClosed);
            if (self->_sinkCounter == 0)
                self->_onClose->complete();
            self->_isClosed = true;
        });
    }

protected:
    Object::Ref<Completer<void>> _onClose;
    Object::Ref<ThreadPool> _callbackHandler;
    size_t _sinkCounter;

    Function<void()> _listener;
    bool _isClosed = false;
};

template <typename T>
class Stream : public Stream<std::nullptr_t>
{
public:
    Stream(Object::Ref<ThreadPool> callbackHandler) : Stream<std::nullptr_t>(callbackHandler), _onClose(Object::create<Completer<void>>(callbackHandler)) {}
    Stream(State<StatefulWidget> *state) : Stream<std::nullptr_t>(state), _onClose(Object::create<Completer<void>>(state)) {}
    virtual ~Stream()
    {
        if (!this->_isClosed)
            this->_onClose->complete();
    }

    virtual Object::Ref<Stream<T>> sink(const T &value)
    {
        Object::Ref<Stream<T>> self = Object::cast<>(this);
        this->_callbackHandler->post([self, value] {
            assert(!self->_isClosed);
            if (self->_listener)
                self->_listener(std::move(value));
            else
                self->_cache.emplace_back(value);
        });
        return self;
    }

    virtual Object::Ref<Stream<T>> sink(T &&value)
    {
        Object::Ref<Stream<T>> self = Object::cast<>(this);
        this->_callbackHandler->post([self, value] {
            assert(!self->_isClosed);
            if (self->_listener)
                self->_listener(std::move(value));
            else
                self->_cache.emplace_back(std::move(value));
        });
        return self;
    }

    virtual Object::Ref<Stream<T>> listen(Function<void(T)> fn)
    {
        Object::Ref<Stream<T>> self = Object::cast<>(this);
        this->_callbackHandler->post([self, fn] {
            assert(!static_cast<bool>(self->_listener) && "Single listener stream can't have more than one listener");
            self->_listener = fn;
            for (auto &cache : self->_cache)
                self->_listener(std::move(cache));
            self->_cache.clear();
            if (self->_isClosed)
                self->_onClose->complete();
        });
        return self;
    }

    virtual Object::Ref<Stream<T>> onClose(Function<void()> fn)
    {
        this->_onClose->future->than(fn);
        return Object::cast<>(this);
    }

    virtual Object::Ref<Future<void>> asFuture()
    {
        return this->_onClose->future;
    }

    void close() override
    {
        Object::Ref<Stream<T>> self = Object::cast<>(this);
        this->_callbackHandler->post([self] {
            assert(!self->_isClosed);
            if (self->_cache.empty())
                self->_onClose->complete();
            self->_isClosed = true;
        });
    }

protected:
    Object::Ref<Completer<void>> _onClose;
    List<T> _cache;

    Function<void(T)> _listener;
    bool _isClosed = false;
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