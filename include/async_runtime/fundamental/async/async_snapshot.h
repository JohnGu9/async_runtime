#pragma once

#include "../async.h"

template <>
class AsyncSnapshot<std::nullptr_t> : public Object, public StateHelper
{
public:
    class ConnectionState
    {
    public:
        enum Value
        {
            none,
            active,
            done
        };
        static const List<Value> values;
        static String toString(Value value);
    };

    virtual bool hasData() = 0;

protected:
    AsyncSnapshot(ConnectionState::Value state) : _state(state) {}
    ConnectionState::Value _state;

    template <typename T>
    static T &getDataFromFuture(option<Future<T>> future)
    {
        return future->_data;
    }

public:
    const ConnectionState::Value &state = _state;

    template <typename T>
    static const std::atomic_bool &getCompletedFromFuture(ref<Future<T>> future)
    {
        return future->_completed;
    }
};

template <>
class AsyncSnapshot<void> : public AsyncSnapshot<std::nullptr_t>
{
public:
    AsyncSnapshot(ref<Future<void>> future)
        : AsyncSnapshot<std::nullptr_t>(future->_completed ? ConnectionState::done : ConnectionState::active) {}
    bool hasData() override { return false; }
};

template <typename T>
class AsyncSnapshot : public AsyncSnapshot<std::nullptr_t>
{
public:
    AsyncSnapshot()
        : AsyncSnapshot<std::nullptr_t>(ConnectionState::none),
          _hasData(false) {}
    AsyncSnapshot(T &&data)
        : _data(data), data(_data),
          AsyncSnapshot<std::nullptr_t>(ConnectionState::active),
          _hasData(true) {}
    AsyncSnapshot(ref<Future<T>> future)
        : data(future->_data),
          AsyncSnapshot<std::nullptr_t>(future->_completed ? ConnectionState::done : ConnectionState::active),
          _hasData(future->_completed) {}

protected:
    option<Future<T>> _future;
    T _data;
    bool _hasData;

public:
    const T &data;
    bool hasData() override { return _hasData; }
};
