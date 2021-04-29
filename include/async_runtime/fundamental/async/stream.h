#pragma once

#include "../async.h"

template <>
class Stream<std::nullptr_t> : public Object, StateHelper
{
    template <typename R>
    friend class StreamSubscription;

    template <typename R>
    friend class AsyncSnapshot;

protected:
    Stream(ref<ThreadPool> callbackHandler) : _callbackHandler(callbackHandler) {}
    Stream(State<StatefulWidget> *state) : _callbackHandler(getHandlerfromState(state)) {}

    ref<ThreadPool> _callbackHandler;

public:
    virtual void close() = 0;
};

template <>
class Stream<void> : public Stream<std::nullptr_t>
{
    template <typename R>
    friend class StreamSubscription;

    template <typename R>
    friend class AsyncSnapshot;

public:
    Stream(ref<ThreadPool> callbackHandler) : Stream<std::nullptr_t>(callbackHandler), _onClose(Object::create<Completer<void>>(callbackHandler)) {}
    Stream(State<StatefulWidget> *state) : Stream<std::nullptr_t>(state), _onClose(Object::create<Completer<void>>(state)) {}
    virtual ~Stream()
    {
        if (!this->_isClosed)
            this->_onClose->completeSync();
    }

    virtual ref<Stream<void>> sink()
    {
        ref<Stream<void>> self = self();
        this->_callbackHandler->post([self] {
            assert(!self->_isClosed);
            if (self->_listener)
                self->_listener();
            else
                self->_sinkCounter++;
        });
        return self;
    }

    virtual ref<StreamSubscription<void>> listen(Function<void()> fn)
    {
        ref<Stream<void>> self = self();
        this->_callbackHandler->post([self, fn] {
            assert(!static_cast<bool>(self->_listener) && "Single listener stream can't have more than one listener");
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

    virtual ref<Future<void>> asFuture()
    {
        return this->_onClose->future;
    }

    void close() override
    {
        ref<Stream<void>> self = self();
        this->_callbackHandler->post([self] {
            assert(!self->_isClosed);
            if (self->_sinkCounter == 0)
                self->_onClose->completeSync();
            self->_isClosed = true;
        });
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
    template <typename R>
    friend class StreamSubscription;

    template <typename R>
    friend class AsyncSnapshot;

public:
    Stream(ref<ThreadPool> callbackHandler)
        : Stream<std::nullptr_t>(callbackHandler),
          _onClose(Object::create<Completer<void>>(callbackHandler)),
          _cache(Object::create<List<T>>()) {}
    Stream(State<StatefulWidget> *state)
        : Stream<std::nullptr_t>(state),
          _onClose(Object::create<Completer<void>>(state)),
          _cache(Object::create<List<T>>()) {}
    virtual ~Stream()
    {
        if (!this->_isClosed)
            this->_onClose->completeSync();
    }

    virtual ref<Stream<T>> sink(const T &value)
    {
        ref<Stream<T>> self = self();
        this->_callbackHandler->post([self, value] {
            assert(!self->_isClosed);
            if (self->_listener)
                self->_listener(std::move(value));
            else
                self->_cache.assertNotNull()->emplace_back(value);
        });
        return self;
    }

    virtual ref<Stream<T>> sink(T &&value)
    {
        ref<Stream<T>> self = self();
        this->_callbackHandler->post([self, value] {
            assert(!self->_isClosed);
            if (self->_listener)
                self->_listener(std::move(value));
            else
                self->_cache.assertNotNull()->emplace_back(std::move(value));
        });
        return self;
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

    virtual ref<Future<void>> asFuture()
    {
        return this->_onClose->future;
    }

    void close() override
    {
        assert(!this->_isClosed);
        ref<Stream<T>> self = self();
        this->_callbackHandler->post([self] {
            assert(!self->_isClosed);
            self->_isClosed = true;
            if (self->_cache == nullptr)
                self->_onClose->completeSync();
        });
    }

protected:
    ref<Completer<void>> _onClose;
    option<List<T>> _cache;

    Function<void(T)> _listener;
    bool _isClosed = false;
};
