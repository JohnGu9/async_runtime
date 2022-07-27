#pragma once
#include "../async.h"

template <>
class StreamSubscription<Object::Void> : public virtual Object
{
    _ASYNC_RUNTIME_FRIEND_ASYNC_FAMILY
public:
    virtual void resume() = 0;
    virtual void pause() = 0;
    virtual void cancel() noexcept = 0;
    virtual bool alive() noexcept = 0;
};

template <typename T>
class StreamSubscription : public StreamSubscription<Object::Void>
{
    _ASYNC_RUNTIME_FRIEND_ASYNC_FAMILY
    static finalref<Fn<void(const T &)>> _useless;

public:
    StreamSubscription(Function<void(const T &)> listener) : _listener(listener) {}
    void resume() override
    {
        RUNTIME_ASSERT(_canceled == false, "StreamSubscription already canceled that can not resume any more. ");
        _alive = true;
        _resume(self());
    }

    void pause() override
    {
        _alive = false;
        _pause(self());
    }

    void cancel() noexcept override
    {
        _alive = false;
        _canceled = true;
        _cancel(self());
        _listener = _useless;
    }

    bool alive() noexcept override
    {
        return !_canceled && _alive;
    }

protected:
    bool _alive = true;
    bool _canceled = false;
    Function<void(const T &)> _listener;
    lateref<Fn<void(ref<StreamSubscription<T>>)>> _resume;
    lateref<Fn<void(ref<StreamSubscription<T>>)>> _pause;
    lateref<Fn<void(ref<StreamSubscription<T>>)>> _cancel;
};

template <typename T>
finalref<Fn<void(const T &)>> StreamSubscription<T>::_useless = [](const T &) {};
