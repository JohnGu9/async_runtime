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
    StreamSubscription(Object::Ref<Stream<T>> stream) : _stream(stream) {}
    void unsubscribe() override
    {
        Object::Ref<StreamSubscription<T>> self = Object::cast<>(this);
        _stream->_callbackHandler->post([self] {
            self->_stream->_listener = nullptr;
        });
    }

protected:
    Object::Ref<Stream<T>> _stream;

public:
    const Object::Ref<Stream<T>> &stream = _stream;
};