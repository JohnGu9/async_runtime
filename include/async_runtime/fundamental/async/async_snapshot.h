#pragma once

#include "../async.h"

template <>
class AsyncSnapshot<std::nullptr_t> : public Object
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
        static ref<String> toString(Value value);
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

template <typename T>
class AsyncSnapshot : public AsyncSnapshot<std::nullptr_t>
{
public:
    AsyncSnapshot()
        : AsyncSnapshot<std::nullptr_t>(ConnectionState::none),
          _hasData(false) {}
    AsyncSnapshot(const T &data, ConnectionState::Value state = ConnectionState::done)
        : AsyncSnapshot<std::nullptr_t>(state),
          _data(data), data(_data),
          _hasData(true) {}
    AsyncSnapshot(T &&data, ConnectionState::Value state = ConnectionState::done)
        : AsyncSnapshot<std::nullptr_t>(state),
          _data(std::move(data)), data(_data),
          _hasData(true) {}
    AsyncSnapshot(ref<Future<T>> future)
        : AsyncSnapshot<std::nullptr_t>(future->_completed ? ConnectionState::done : ConnectionState::active),
          _hasData(future->_completed), data(future->_data) {}

protected:
    T _data;
    bool _hasData;

public:
    const T &data;
    bool hasData() override { return _hasData; }
};
