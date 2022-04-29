#pragma once

#include "../async.h"

template <>
class StreamSubscription<std::nullptr_t> : public virtual Object
{
    _ASYNC_RUNTIME_FRIEND_ASYNC_FAMILY
public:
    virtual void resume() = 0;
    virtual void pause() = 0;
    virtual void cancel() = 0;
    virtual bool alive() = 0;
};

template <typename T>
class StreamSubscription : public StreamSubscription<std::nullptr_t>
{
    _ASYNC_RUNTIME_FRIEND_ASYNC_FAMILY
public:
    StreamSubscription(Function<void(const T &)> listener) : _listener(listener) {}
    void resume() override
    {
        assert(_canceled == false && "StreamSubscription already canceled that can not resume. ");
        _alive = true;
    }

    void pause() override
    {
        _alive = false;
    }

    void cancel() override
    {
        _canceled = true;
        _cancel();
    }

    bool alive() override
    {
        return !_canceled && _alive;
    }

protected:
    bool _alive = true;
    bool _canceled = false;
    Function<void(const T &)> _listener;
    lateref<Fn<void()>> _cancel;
};
