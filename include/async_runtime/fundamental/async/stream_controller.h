#pragma once

#include "../async.h"
#include "stream.h"

template <typename T = std::nullptr_t>
class AsyncStreamController;

template <>
class StreamController<std::nullptr_t> : public Object, protected StateHelper
{
public:
    virtual void close() = 0;
};

template <>
class StreamController<void> : public StreamController<std::nullptr_t>
{
    ref<Stream<void>> _stream;

public:
    StreamController(ref<ThreadPool> handler) : _stream(Object::create<Stream<void>>(handler)) {}
    StreamController(ref<State<StatefulWidget>> state) : _stream(Object::create<Stream<void>>(getHandlerfromState(state))) {}

    const ref<Stream<void>> &stream = _stream;

    virtual void sink()
    {
        assert(!_stream->_isClosed);
        if (_stream->_listener)
            _stream->_listener();
        else
            _stream->_sinkCounter++;
    }

    void close() override
    {
        assert(!_stream->_isClosed);
        _stream->_isClosed = true;
        if (_stream->_sinkCounter == 0)
            _stream->_onClose->completeSync();
    }
};

template <typename T>
class StreamController : public StreamController<std::nullptr_t>
{
    ref<Stream<T>> _stream;

public:
    StreamController(ref<ThreadPool> handler) : _stream(Object::create<Stream<T>>(handler)) {}
    StreamController(ref<State<StatefulWidget>> state) : _stream(Object::create<Stream<T>>(StateHelper::getHandlerfromState(state))) {}

    const ref<Stream<T>> &stream = _stream;

    virtual void sink(const T &value)
    {
        assert(!_stream->_isClosed);
        if (_stream->_listener)
            _stream->_listener(value);
        else
            _stream->_cache.assertNotNull()->emplace_back(value);
    }

    virtual void sink(T &&value)
    {
        assert(!_stream->_isClosed);
        if (_stream->_listener)
            _stream->_listener(std::move(value));
        else
            _stream->_cache.assertNotNull()->emplace_back(std::move(value));
    }

    void close() override
    {
        assert(!_stream->_isClosed);
        _stream->_isClosed = true;
        if (_stream->_cache == nullptr)
            _stream->_onClose->completeSync();
    }
};

template <>
class AsyncStreamController<void> : public StreamController<void>
{
    ref<ThreadPool> _handler;

public:
    AsyncStreamController(ref<ThreadPool> handler)
        : StreamController<void>(handler), _handler(handler) {}
    AsyncStreamController(ref<State<StatefulWidget>> state)
        : StreamController<void>(state), _handler(StateHelper::getHandlerfromState(state)) {}

    void sink() override
    {
        ref<StreamController<void>> self = self();
        _handler->post([this, self] { this->StreamController<void>::sink(); });
    }

    void close() override
    {
        ref<StreamController<void>> self = self();
        _handler->post([this, self] { this->StreamController<void>::close(); });
    }
};

template <typename T>
class AsyncStreamController : public StreamController<T>
{
    ref<ThreadPool> _handler;

public:
    AsyncStreamController(ref<ThreadPool> handler)
        : StreamController<T>(handler), _handler(handler) {}
    AsyncStreamController(ref<State<StatefulWidget>> state)
        : StreamController<T>(state), _handler(StateHelper::getHandlerfromState(state)) {}

    void sink(const T &value) override
    {
        ref<AsyncStreamController<T>> self = self();
        this->_handler->post([this, self, value] { this->StreamController<T>::sink(std::move(value)); });
    }

    void sink(T &&value) override
    {
        ref<AsyncStreamController<T>> self = self();
        this->_handler->post([this, self, value] { this->StreamController<T>::sink(std::move(value)); });
    }

    void close() override
    {
        ref<AsyncStreamController<T>> self = self();
        this->_handler->post([this, self] { this->StreamController<T>::close(); });
    }
};
