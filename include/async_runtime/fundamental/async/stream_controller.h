#pragma once

#include "../async.h"
#include "stream.h"

template <>
class StreamController<std::nullptr_t> : public Object, protected StateHelper
{
protected:
    static void completeSync(ref<Completer<void>> onClose)
    {
        onClose->completeSync();
    }

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
        lateref<Fn<void()>> listener;
        if (_stream->_listener.isNotNull(listener))
            listener();
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
        lateref<Fn<void(const T &)>> listener;
        if (_stream->_listener.isNotNull(listener))
            listener(value);
        else
            _stream->_cache->emplace_back(value);
    }

    virtual void sink(T &&value)
    {
        assert(!_stream->_isClosed);
        lateref<Fn<void(const T &)>> listener;
        if (_stream->_listener.isNotNull(listener))
            listener(value);
        else
            _stream->_cache->emplace_back(std::move(value));
    }

    void close() override
    {
        assert(!_stream->_isClosed);
        _stream->_isClosed = true;
        if (_stream->_cache->empty())
            _stream->_onClose->completeSync();
    }
};

template <typename T>
class BroadcastStreamController;

template <>
class BroadcastStreamController<void> : public StreamController<std::nullptr_t>
{
    ref<BroadcastStream<void>> _stream;

public:
    BroadcastStreamController(ref<ThreadPool> handler) : _stream(Object::create<BroadcastStream<void>>(handler)) {}
    BroadcastStreamController(ref<State<StatefulWidget>> state) : _stream(Object::create<BroadcastStream<void>>(getHandlerfromState(state))) {}

    const ref<BroadcastStream<void>> &stream = _stream;

    virtual void sink()
    {
        assert(!_stream->_isClosed);
        lateref<Fn<void()>> listener;
        if (!_stream->_listeners->empty())
            for (auto &fn : _stream->_listeners)
                fn();
        else
            _stream->_sinkCounter++;
    }

    void close() override
    {
        assert(!_stream->_isClosed);
        _stream->_isClosed = true;
        if (_stream->_sinkCounter == 0)
            completeSync(_stream->_onClose);
    }
};

template <typename T>
class BroadcastStreamController : public StreamController<std::nullptr_t>
{
    ref<BroadcastStream<T>> _stream;

public:
    BroadcastStreamController(ref<ThreadPool> handler) : _stream(Object::create<BroadcastStream<T>>(handler)) {}
    BroadcastStreamController(ref<State<StatefulWidget>> state) : _stream(Object::create<BroadcastStream<T>>(StateHelper::getHandlerfromState(state))) {}

    const ref<BroadcastStream<T>> &stream = _stream;

    virtual void sink(const T &value)
    {
        assert(!_stream->_isClosed);
        if (!_stream->_listeners->empty())
            for (auto &fn : _stream->_listeners)
                fn(value);
        else
            _stream->_cache->emplace_back(value);
    }

    virtual void sink(T &&value)
    {
        assert(!_stream->_isClosed);
        lateref<Fn<void(T)>> listener;
        if (!_stream->_listeners->empty())
            for (auto &fn : _stream->_listeners)
                fn(value);
        else
            _stream->_cache->emplace_back(std::move(value));
    }

    void close() override
    {
        assert(!_stream->_isClosed);
        _stream->_isClosed = true;
        if (_stream->_cache->empty())
            completeSync(_stream->_onClose);
    }
};

// @ thread safe
template <typename T = std::nullptr_t>
class AsyncStreamController;

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
        _handler->post([this, self]
                       { this->StreamController<void>::sink(); });
    }

    void close() override
    {
        ref<StreamController<void>> self = self();
        _handler->post([this, self]
                       { this->StreamController<void>::close(); });
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
        this->_handler->post([this, self, value]
                             { this->StreamController<T>::sink(std::move(value)); });
    }

    void sink(T &&value) override
    {
        ref<AsyncStreamController<T>> self = self();
        this->_handler->post([this, self, value]
                             { this->StreamController<T>::sink(std::move(value)); });
    }

    virtual void sinkSync(const T &value)
    {
        this->_handler->post([this, &value]
                             { this->StreamController<T>::sink(value); })
            .get();
    }

    virtual void sinkSync(T &&value)
    {
        this->_handler->post([this, &value]
                             { this->StreamController<T>::sink(std::move(value)); })
            .get();
    }

    void close() override
    {
        ref<AsyncStreamController<T>> self = self();
        this->_handler->post([this, self]
                             { this->StreamController<T>::close(); });
    }
};
