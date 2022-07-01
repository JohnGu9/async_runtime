#pragma once

#include "../async.h"

template <>
class AsyncSnapshot<Object::Void> : public virtual Object
{
public:
    class ConnectionState
    {
    public:
        enum Value : int
        {
            none,
            active,
            done
        };
        static const ref<List<Value>> values;
        static ref<String> toString(Value value);
    };

    virtual bool hasData() noexcept = 0;

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
};

namespace std
{
    /**
     * @brief Before C++14, there is no standard for enum convert to hash.
     * In Windows and Linux, system provide the way.
     * But Macos doesn't.
     *
     */
    template <>
    struct hash<::AsyncSnapshot<>::ConnectionState::Value>
    {
        std::size_t operator()(const ::AsyncSnapshot<>::ConnectionState::Value &other) const
        {
            return ::std::hash<int>()(static_cast<int>(other));
        }
    };
};

std::ostream &operator<<(std::ostream &os, const AsyncSnapshot<>::ConnectionState::Value &value);

template <typename T>
class AsyncSnapshot : public AsyncSnapshot<Object::Void>
{
public:
    static ref<AsyncSnapshot<T>> noData(ConnectionState::Value state = ConnectionState::done);
    static ref<AsyncSnapshot<T>> fromFuture(ref<Future<T>> future);
    static ref<AsyncSnapshot<T>> hasData(const T &data, ConnectionState::Value state = ConnectionState::done);
    static ref<AsyncSnapshot<T>> hasData(T &&data, ConnectionState::Value state = ConnectionState::done);
    static ref<AsyncSnapshot<T>> stateWrapper(ref<AsyncSnapshot<T>>, ConnectionState::Value state);

    AsyncSnapshot(ConnectionState::Value state = ConnectionState::none)
        : AsyncSnapshot<Object::Void>(state) {}

protected:
    class _HasDataAsyncSnapshot;
    class _FromFuture;
    class _StateWrapper;

public:
    virtual const T &data() { throw std::runtime_error("AsyncSnapshot has no data"); };
    bool hasData() noexcept override { return false; }
};

template <typename T>
class AsyncSnapshot<T>::_HasDataAsyncSnapshot : public AsyncSnapshot<T>
{
    using super = AsyncSnapshot<T>;

protected:
    T _data;

public:
    _HasDataAsyncSnapshot(const T &data, ConnectionState::Value state) : super(state), _data(data) {}
    _HasDataAsyncSnapshot(T &&data, ConnectionState::Value state) : super(state), _data(std::move(data)) {}
    const T &data() noexcept override { return _data; };
    bool hasData() noexcept override { return true; }
};

template <typename T>
class AsyncSnapshot<T>::_StateWrapper : public AsyncSnapshot<T>
{
    using super = AsyncSnapshot<T>;

protected:
    ref<AsyncSnapshot<T>> _data;

public:
    _StateWrapper(ref<AsyncSnapshot<T>> other, ConnectionState::Value state)
        : super(state), _data(other) {}

    const T &data() override { return _data->data(); };
    bool hasData() noexcept override { return _data->hasData(); }
};

template <typename T>
ref<AsyncSnapshot<T>> AsyncSnapshot<T>::noData(ConnectionState::Value state)
{
    return Object::create<AsyncSnapshot<T>>(state);
}

template <typename T>
ref<AsyncSnapshot<T>> AsyncSnapshot<T>::hasData(const T &data, ConnectionState::Value state)
{
    return Object::create<AsyncSnapshot<T>::_HasDataAsyncSnapshot>(data, state);
}

template <typename T>
ref<AsyncSnapshot<T>> AsyncSnapshot<T>::hasData(T &&data, ConnectionState::Value state)
{
    return Object::create<AsyncSnapshot<T>::_HasDataAsyncSnapshot>(std::move(data), state);
}

template <typename T>
ref<AsyncSnapshot<T>> AsyncSnapshot<T>::stateWrapper(ref<AsyncSnapshot<T>> other, ConnectionState::Value state)
{
    return Object::create<AsyncSnapshot<T>::_StateWrapper>(other, state);
}
