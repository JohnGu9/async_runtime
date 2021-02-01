#pragma once

#include "../async.h"

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
            this->_onClose->completeSync();
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
                self->_onClose->completeSync();
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
                self->_onClose->completeSync();
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
            this->_onClose->completeSync();
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
                self->_onClose->completeSync();
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
                self->_onClose->completeSync();
            self->_isClosed = true;
        });
    }

protected:
    Object::Ref<Completer<void>> _onClose;
    List<T> _cache;

    Function<void(T)> _listener;
    bool _isClosed = false;
};
