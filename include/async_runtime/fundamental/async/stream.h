#pragma once

#include "../async.h"

template <>
class Stream<std::nullptr_t> : public Object, StateHelper
{
    _ASYNC_RUNTIME_FRIEND_ASYNC_FAMILY;

protected:
    Stream(ref<ThreadPool> callbackHandler) : _callbackHandler(callbackHandler) {}

    ref<ThreadPool> _callbackHandler;

public:
    virtual ref<Future<void>> asFuture() = 0;
};

template <>
class Stream<void> : public Stream<std::nullptr_t>
{
    _ASYNC_RUNTIME_FRIEND_ASYNC_FAMILY;

public:
    Stream(ref<ThreadPool> callbackHandler) : Stream<std::nullptr_t>(callbackHandler), _onClose(Object::create<Completer<void>>(callbackHandler)) {}
    virtual ~Stream()
    {
        if (!this->_isClosed)
            this->_onClose->completeSync();
    }

    virtual ref<StreamSubscription<void>> listen(Function<void()> fn)
    {
        ref<Stream<void>> self = self();
        this->_callbackHandler->post([self, fn] {
            assert(self->_listener == nullptr && "Single listener stream can't have more than one listener");
            self->_listener = fn;
            for (size_t c = 0; c < self->_sinkCounter; c++)
                self->_listener();
            self->_sinkCounter = 0;
            if (self->_isClosed)
                self->_onClose->completeSync();
        });
        return Object::create<StreamSubscription<void>>(self);
    }

    virtual ref<Stream<void>> onClose(Function<void()> fn)
    {
        this->_onClose->future->than(fn);
        return self();
    }

    ref<Future<void>> asFuture() override
    {
        return this->_onClose->future;
    }

protected:
    ref<Completer<void>> _onClose;
    size_t _sinkCounter = 0;

    Function<void()> _listener;
    bool _isClosed = false;
};

template <typename T>
class Stream : public Stream<std::nullptr_t>
{
    _ASYNC_RUNTIME_FRIEND_ASYNC_FAMILY;

public:
    Stream(ref<ThreadPool> callbackHandler)
        : Stream<std::nullptr_t>(callbackHandler),
          _onClose(Object::create<Completer<void>>(callbackHandler)),
          _cache(Object::create<List<T>>()) {}
    virtual ~Stream()
    {
        if (!this->_isClosed)
            this->_onClose->completeSync();
    }

    virtual ref<StreamSubscription<T>> listen(Function<void(T)> fn)
    {
        assert(!static_cast<bool>(this->_listener) && "Single listener stream can't have more than one listener");
        ref<Stream<T>> self = self();
        this->_callbackHandler->post([self, fn] {
            assert(!static_cast<bool>(self->_listener) && "Single listener stream can't have more than one listener");
            self->_listener = fn;
            for (auto &cache : self->_cache.assertNotNull())
                self->_listener(std::move(cache));
            self->_cache = nullptr;
            if (self->_isClosed)
                self->_onClose->completeSync();
        });
        return Object::create<StreamSubscription<T>>(self);
    }

    virtual ref<Stream<T>> onClose(Function<void()> fn)
    {
        this->_onClose->future->than(fn);
        return self();
    }

    ref<Future<void>> asFuture() override
    {
        return this->_onClose->future;
    }

protected:
    ref<Completer<void>> _onClose;
    option<List<T>> _cache;

    Function<void(T)> _listener;
    bool _isClosed = false;
};
