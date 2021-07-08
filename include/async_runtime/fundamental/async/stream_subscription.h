#pragma once

#include "../async.h"

template <>
class StreamSubscription<std::nullptr_t> : public Object
{
public:
    virtual void resume() = 0;
    virtual void pause() = 0;
    virtual void cancel() = 0;
};

template <typename T>
class StreamSubscription : public StreamSubscription<std::nullptr_t>
{
public:
    StreamSubscription(Function<void()> resume, Function<void()> unsubscribe) : _resume(resume), _unsubscribe(unsubscribe) {}
    void resume() override
    {
        _resume();
    }
    void pause() override
    {
        _unsubscribe();
    }
    void cancel() override
    {
        _unsubscribe();
        _unsubscribe = [] {};
    }

protected:
    Function<void()> _resume;
    Function<void()> _unsubscribe;
};
