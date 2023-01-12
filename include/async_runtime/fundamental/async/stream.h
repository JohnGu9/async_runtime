#pragma once
#include "../async.h"

template <>
class Stream<Object::Void> : public virtual Object, public EventLoopGetterMixin
{
    _ASYNC_RUNTIME_FRIEND_ASYNC_FAMILY;

protected:
    Stream(ref<EventLoop> loop) : _loop(loop), _onClose(Object::create<Future<int>>(loop)) {}

    ref<EventLoop> _loop;
    ref<Future<int>> _onClose;
    bool _isClosed = false;

public:
    ref<EventLoop> eventLoop() override { return _loop; }

    virtual bool isClosed() const { return _isClosed; }
    virtual ref<Future<int>> asFuture() { return _onClose; }

    virtual ~Stream()
    {
        DEBUG_ASSERT(_isClosed && "Stream must be closed before dispose");
    }
};

template <typename T>
class Stream : public Stream<Object::Void>
{
    _ASYNC_RUNTIME_FRIEND_ASYNC_FAMILY;

    static finalref<Fn<void(ref<StreamSubscription<T>>)>> _useless;
    static finalref<Fn<Function<void(const T &)>(const ref<StreamSubscription<T>> &)>> _extractListeners;

public:
    Stream(option<EventLoopGetterMixin> getter = nullptr)
        : Stream<Object::Void>(EventLoopGetterMixin::ensureEventLoop(getter)) {}

    virtual ref<StreamSubscription<T>> listen(Function<void(const T &)> fn);

protected:
    virtual void sink(T value);
    virtual void close() noexcept;

    finalref<List<T>> _cache = List<T>::create();
    finalref<Set<ref<StreamSubscription<T>>>> _listeners = Set<ref<StreamSubscription<T>>>::create();
    finalref<Set<ref<StreamSubscription<T>>>> _active = Set<ref<StreamSubscription<T>>>::create();

    Function<void(ref<StreamSubscription<T>>)> _resume = [this](ref<StreamSubscription<T>> subscription) { //
        this->_active->emplace(subscription);
    };

    Function<void(ref<StreamSubscription<T>>)> _pause = [this](ref<StreamSubscription<T>> subscription) { //
        this->_active->erase(this->_active->find(subscription));
    };

    Function<void(ref<StreamSubscription<T>>)> _cancel = [this](ref<StreamSubscription<T>> subscription) { //
        this->_active->erase(this->_active->find(subscription));
        this->_listeners->erase(this->_active->find(subscription));
        subscription->_resume = _useless;
        subscription->_pause = _useless;
        subscription->_cancel = _useless;
    };

    void rawClose();
};

template <typename T>
class StreamController : public Stream<T>
{
    _ASYNC_RUNTIME_FRIEND_ASYNC_FAMILY;
    using super = Stream<T>;

public:
    StreamController(option<EventLoopGetterMixin> getter = nullptr) : super(getter) {}
    void sink(T value) override { super::sink(std::move(value)); }
    void close() noexcept override { super::close(); }
};

template <typename T>
finalref<Fn<void(ref<StreamSubscription<T>>)>> Stream<T>::_useless = [](ref<StreamSubscription<T>>) {};

template <typename T>
finalref<Fn<Function<void(const T &)>(const ref<StreamSubscription<T>> &)>> Stream<T>::_extractListeners =
    [](const ref<StreamSubscription<T>> &subscription) { //
        return subscription->_listener;
    };

template <typename T>
void Stream<T>::rawClose()
{
    this->_onClose->markAsCompleted();
    this->_active->clear();
    for (auto &listener : this->_listeners)
    {
        listener->_resume = _useless;
        listener->_pause = _useless;
        listener->_cancel = _useless;
        listener->cancel();
    }
    this->_listeners->clear();
}

template <typename T>
ref<StreamSubscription<T>> Stream<T>::listen(Function<void(const T &)> fn)
{
    RUNTIME_ASSERT(!_isClosed, "Listen on a closed Stream");
    ref<Stream<T>> self = self();
    auto subscription = Object::create<StreamSubscription<T>>(fn);
    subscription->_resume = this->_resume;
    subscription->_pause = this->_pause;
    subscription->_cancel = this->_cancel;
    _active->emplace(subscription);
    _listeners->emplace(subscription);
    _loop->callSoon([this, self] { //
        for (auto iter = this->_cache->begin(); iter != this->_cache->end();)
        {
            if (!this->_active->isEmpty())
            {
                auto copy = this->_active->copy();
                for (auto &listener : copy)
                    listener->_listener(*iter);
                iter = this->_cache->erase(iter);
            }
            else
                break;
        }
        if (this->_isClosed && _cache->isEmpty() && !this->_onClose->completed())
            this->rawClose();
    });
    return subscription;
}

template <typename T>
void Stream<T>::sink(T value)
{
    RUNTIME_ASSERT(!_isClosed, "Sink on a closed Stream");
    if (!this->_active->isEmpty())
    {
        auto copy = this->_active->template map<Function<void(const T &)>>(_extractListeners);
        for (const auto &listener : copy)
            listener(value);
    }
    else
        this->_cache->emplaceBack(std::move(value));
}

template <typename T>
void Stream<T>::close() noexcept
{
    if (!_isClosed)
    {
        _isClosed = true;
        if (_cache->isEmpty() && !this->_onClose->completed())
            this->rawClose();
    }
}
