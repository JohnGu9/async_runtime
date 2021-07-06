#pragma once

#include "../async.h"

template <>
class StreamSubscription<std::nullptr_t> : public Object
{
public:
    virtual void cancel() = 0;
};

template <typename T>
class StreamSubscription : public StreamSubscription<std::nullptr_t>
{
public:
    StreamSubscription(Function<void()> unsubscribe) : _unsubscribe(unsubscribe) {}
    void cancel() override
    {
        _unsubscribe();
        _unsubscribe = [] {};
    }

protected:
    Function<void()> _unsubscribe;
};