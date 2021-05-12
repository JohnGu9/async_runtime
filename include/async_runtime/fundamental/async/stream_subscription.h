#pragma once

#include "../async.h"

template <>
class StreamSubscription<std::nullptr_t> : public Object
{
public:
    virtual void unsubscribe() = 0;
};

template <typename T>
class StreamSubscription : public StreamSubscription<std::nullptr_t>
{
public:
    StreamSubscription(ref<Stream<T>> stream) : _stream(stream) {}
    void unsubscribe() override
    {
        ref<StreamSubscription<T>> self = self();
        _stream->_callbackHandler->post([=] {
            self->_stream->_listener = nullptr;
        });
    }

protected:
    ref<Stream<T>> _stream;

public:
    const ref<Stream<T>> &stream = _stream;
};