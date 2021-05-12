#pragma once

#include "../async.h"

template <>
class Stream<std::nullptr_t> : public Object, StateHelper
{
    _ASYNC_RUNTIME_FRIEND_ASYNC_FAMILY;

protected:
    Stream(ref<ThreadPool> callbackHandler) : _callbackHandler(callbackHandler), _onClose(Object::create<Completer<void>>(callbackHandler)) {}

    ref<ThreadPool> _callbackHandler;
    ref<Completer<void>> _onClose;
    bool _isClosed = false;

public:
    virtual ref<Future<void>> asFuture()
    {
        return this->_onClose->future;
    }

    virtual ~Stream()
    {
        if (!this->_isClosed)
            this->_onClose->completeSync();
    }
};

template <>
class Stream<void> : public Stream<std::nullptr_t>
{
    _ASYNC_RUNTIME_FRIEND_ASYNC_FAMILY;

public:
    Stream(ref<ThreadPool> callbackHandler)
        : Stream<std::nullptr_t>(callbackHandler) {}

    virtual ref<StreamSubscription<void>> listen(Function<void()> fn)
    {
        ref<Stream<void>> self = self();
        this->_callbackHandler->post([this, self, fn] {
            assert(this->_listener == nullptr && "Single listener stream can't have more than one listener");
            this->_listener = fn;
            for (size_t c = 0; c < this->_sinkCounter; c++)
                this->_listener();
            this->_sinkCounter = 0;
            if (this->_isClosed)
                this->_onClose->completeSync();
        });
        return Object::create<StreamSubscription<void>>(self);
    }

    virtual ref<Stream<void>> onClose(Function<void()> fn)
    {
        this->_onClose->future->than(fn);
        return self();
    }

protected:
    size_t _sinkCounter = 0;
    Function<void()> _listener;
};

template <typename T>
class Stream : public Stream<std::nullptr_t>
{
    _ASYNC_RUNTIME_FRIEND_ASYNC_FAMILY;

public:
    Stream(ref<ThreadPool> callbackHandler)
        : Stream<std::nullptr_t>(callbackHandler),
          _cache(Object::create<List<T>>()) {}

    virtual ref<StreamSubscription<T>> listen(Function<void(T)> fn)
    {
        ref<Stream<T>> self = self();
        this->_callbackHandler->post([this, self, fn] {
            assert(this->_listener == nullptr && "Single listener stream can't have more than one listener");
            this->_listener = fn;
            for (auto &cache : this->_cache)
                this->_listener(std::move(cache));
            this->_cache->clear();
            if (this->_isClosed)
                this->_onClose->completeSync();
        });
        return Object::create<StreamSubscription<T>>(self);
    }

    virtual ref<Stream<T>> onClose(Function<void()> fn)
    {
        this->_onClose->future->than(fn);
        return self();
    }

protected:
    ref<List<T>> _cache;
    Function<void(T)> _listener;
};
