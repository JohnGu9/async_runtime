#pragma once

#include "../async.h"

template <>
class Stream<std::nullptr_t> : public virtual Object, public EventLoopGetterMixin
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
        if (!this->_onClose->completed())
            this->_onClose->complete(0);
    }
};

template <typename T>
class Stream : public Stream<std::nullptr_t>
{
    _ASYNC_RUNTIME_FRIEND_ASYNC_FAMILY;

    static Function<void(ref<StreamSubscription<T>>)> _useless()
    {
        static Function<void(ref<StreamSubscription<T>>)> useless = [](ref<StreamSubscription<T>>) {};
        return useless;
    }

public:
    Stream(option<EventLoopGetterMixin> getter = nullptr)
        : Stream<std::nullptr_t>(EventLoopGetterMixin::ensureEventLoop(getter)) {}

    virtual ref<StreamSubscription<T>> listen(Function<void(const T &)> fn);

protected:
    virtual void sink(T value);
    virtual void close();

    finalref<List<T>> _cache = Object::create<List<T>>();
    finalref<Set<ref<StreamSubscription<T>>>> _listeners = Object::create<Set<ref<StreamSubscription<T>>>>();
    finalref<Set<ref<StreamSubscription<T>>>> _active = Object::create<Set<ref<StreamSubscription<T>>>>();

    Function<void(ref<StreamSubscription<T>>)> _resume = [this](ref<StreamSubscription<T>> subscription)
    {
        this->_active->insert(subscription);
    };
    Function<void(ref<StreamSubscription<T>>)> _pause = [this](ref<StreamSubscription<T>> subscription)
    {
        this->_active->erase(this->_active->find(subscription));
    };
    Function<void(ref<StreamSubscription<T>>)> _cancel = [this](ref<StreamSubscription<T>> subscription)
    {
        this->_active->erase(this->_active->find(subscription));
        this->_listeners->erase(this->_active->find(subscription));
        subscription->_resume = _useless();
        subscription->_pause = _useless();
        subscription->_cancel = _useless();
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
    void close() override { super::close(); }
};

template <typename T>
void Stream<T>::rawClose()
{
    this->_onClose->complete(0);
    this->_active->clear();
    for (auto &listener : this->_listeners)
    {
        listener->_resume = _useless();
        listener->_pause = _useless();
        listener->_cancel = _useless();
        listener->cancel();
    }
    this->_listeners->clear();
}

template <typename T>
ref<StreamSubscription<T>> Stream<T>::listen(Function<void(const T &)> fn)
{
    assert(!_isClosed);
    ref<Stream<T>> self = self();
    auto subscription = Object::create<StreamSubscription<T>>(fn);
    subscription->_resume = this->_resume;
    subscription->_pause = this->_pause;
    subscription->_cancel = this->_cancel;
    _active->insert(subscription);
    _listeners->insert(subscription);
    _loop->callSoon([this, self] //
                    {            //
                        for (auto iter = this->_cache->begin(); iter != this->_cache->end();)
                        {
                            if (!this->_active->empty())
                            {
                                auto copy = this->_active->copy();
                                for (auto &listener : copy)
                                    listener->_listener(*iter);
                                iter = this->_cache->erase(iter);
                            }
                            else
                                break;
                        }
                        if (this->_isClosed && _cache->empty() && !this->_onClose->completed())
                            this->rawClose();
                    });
    return subscription;
}

template <typename T>
void Stream<T>::sink(T value)
{
    assert(!_isClosed);
    if (!this->_active->empty())
    {
        auto copy = this->_active->copy();
        for (const auto &listener : copy)
            listener->_listener(value);
    }
    else
        this->_cache->emplace_back(std::move(value));
}

template <typename T>
void Stream<T>::close()
{
    assert(!_isClosed);
    _isClosed = true;
    if (_cache->empty() && !this->_onClose->completed())
        this->rawClose();
}
