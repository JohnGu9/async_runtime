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

    ref<EventLoop> eventLoop() override { return _loop; }

public:
    virtual bool isClosed() const { return _isClosed; }
    virtual ref<Future<int>> asFuture() { return _onClose; }

    virtual ~Stream()
    {
        if (!this->_isClosed)
            this->_onClose->resolve(0);
    }
};

template <typename T>
class Stream : public Stream<std::nullptr_t>
{
    _ASYNC_RUNTIME_FRIEND_ASYNC_FAMILY;

public:
    Stream(option<EventLoopGetterMixin> getter = nullptr)
        : Stream<std::nullptr_t>(EventLoopGetterMixin::ensureEventLoop(getter)), _cache(Object::create<List<T>>()) {}

    virtual ref<StreamSubscription<T>> listen(Function<void(const T &)> fn);

protected:
    virtual void sink(T value);
    virtual void close();

    ref<List<T>> _cache;
    ref<Set<ref<StreamSubscription<T>>>> _listeners;
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
ref<StreamSubscription<T>> Stream<T>::listen(Function<void(const T &)> fn)
{
    assert(!_isClosed);
    ref<Stream<T>> self = self();
    auto subscription = Object::create<StreamSubscription<T>>(fn);
    subscription->_cancel = [this, subscription]
    {
        this->_listeners->erase(this->_listeners->find(subscription));
        subscription->_cancel = [] {};
    };
    _listeners->insert(subscription);
    _loop->callSoon([this, self]
                    {
                            for (auto &cache : this->_cache)
                                for (auto &listener : this->_listeners)
                                    listener->_listener(cache);
                            this->_cache->clear();
                            if (this->_isClosed) {
                                this->_onClose->resolve(0);
                                for (auto &listener : this->_listeners)
                                    listener->_cancel = [] {};
                                this->_listeners->clear();
                                } });
    return subscription;
}

template <typename T>
void Stream<T>::sink(T value)
{
    assert(!_isClosed);
    auto self = self();
    _loop->callSoon([this, self, value]
                    {
        if (!this->_listeners->empty())
            for (auto &listener : this->_listeners)
                listener->_listener(value);
        else
            _cache->emplace_back(std::move(value)); });
}

template <typename T>
void Stream<T>::close()
{
    assert(!_isClosed);
    _isClosed = true;
    if (_cache->empty())
    {
        _onClose->resolve(0);
        for (auto &listener : this->_listeners)
            listener->_cancel = [] {};
        this->_listeners->clear();
    }
}
