#pragma once

#include "../async.h"

template <>
class AsyncSnapshot<std::nullptr_t> : public Object, StateHelper
{
public:
    class ConnectionState
    {
        enum Value
        {
            none,
            active,
            done,
            waiting
        };
        static const List<Value> values;
        static String toString(Value value);
    };
};

template <>
class AsyncSnapshot<void> : public Object, StateHelper
{
};

template <typename T>
class AsyncSnapshot : public Object, StateHelper
{
};
