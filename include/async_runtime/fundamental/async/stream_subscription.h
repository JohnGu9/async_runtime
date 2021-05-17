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
    StreamSubscription(Function<void()> unsubscribe) : _unsubscribe(unsubscribe) {}
    void unsubscribe() override
    {
        _unsubscribe();
        _unsubscribe = [] {};
    }
    Function<void()> _unsubscribe;
};