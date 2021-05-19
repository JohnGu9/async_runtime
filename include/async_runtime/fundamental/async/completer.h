#pragma once

#include "../async.h"

template <>
class Completer<std::nullptr_t> : public Object, protected StateHelper
{
    _ASYNC_RUNTIME_FRIEND_ASYNC_FAMILY;

protected:
    Completer(ref<ThreadPool> callbackHandler) : _callbackHandler(callbackHandler), _isCompleted(false), _isCancelled(false) {}
    Completer(ref<State<StatefulWidget>> state) : _callbackHandler(getHandlerfromState(state)), _isCompleted(false), _isCancelled(false) {}

    ref<ThreadPool> _callbackHandler;
    std::atomic_bool _isCompleted;
    std::atomic_bool _isCancelled;

public:
    virtual void cancel() = 0;
    const std::atomic_bool &isCompleted = _isCompleted;
    const std::atomic_bool &isCancelled = _isCancelled;
};

template <>
class Completer<void> : public Completer<std::nullptr_t>
{
    _ASYNC_RUNTIME_FRIEND_ASYNC_FAMILY;

    template <typename R>
    friend ref<Future<R>> async(ref<ThreadPool> callbackHandler, Function<R()> fn);

    template <typename R>
    friend ref<Future<R>> async(ref<State<StatefulWidget>> state, Function<R()> fn);

public:
    Completer(ref<ThreadPool> callbackHandler) : Completer<std::nullptr_t>(callbackHandler), _future(Object::create<Future<void>>(callbackHandler)) {}
    Completer(ref<State<StatefulWidget>> state) : Completer<std::nullptr_t>(state), _future(Object::create<Future<void>>(getHandlerfromState(state))) {}

    virtual void complete()
    {
        ref<Completer<void>> self = self();
        this->_callbackHandler->post([this, self] { this->completeSync(); });
    }

    void cancel() override
    {
        ref<Completer<void>> self = self();
        this->_callbackHandler->post([this, self] {
            if (this->_isCompleted)
                return false; // already completed
            this->_isCancelled = true;
            return true; // cancel successfully
        });
    }

protected:
    virtual void completeSync()
    {
        assert(this->_isCompleted == false);
        if (this->_isCancelled)
            return;
        this->_future->_completed = true;
        for (auto &fn : this->_future->_callbackList)
            fn();
        this->_future->_callbackList->clear();
    }

    ref<Future<void>> _future;

public:
    const ref<Future<void>> &future = _future;
};

template <typename T>
class Completer : public Completer<std::nullptr_t>
{
    _ASYNC_RUNTIME_FRIEND_ASYNC_FAMILY;

    template <typename R>
    friend ref<Future<R>> async(ref<ThreadPool> callbackHandler, Function<R()> fn);

    template <typename R>
    friend ref<Future<R>> async(ref<State<StatefulWidget>> state, Function<R()> fn);

public:
    Completer(ref<ThreadPool> callbackHandler) : Completer<std::nullptr_t>(callbackHandler), _future(Object::create<Future<T>>(callbackHandler)) {}
    Completer(ref<State<StatefulWidget>> state) : Completer<std::nullptr_t>(state), _future(Object::create<Future<T>>(StateHelper::getHandlerfromState(state))) {}

    virtual void complete(const T &value)
    {
        ref<Completer<T>> self = self();
        this->_callbackHandler->post([this, self, value] { this->completeSync((value)); });
    }

    virtual void complete(T &&value)
    {
        ref<Completer<T>> self = self();
        this->_callbackHandler->post([this, self, value] { this->completeSync(value); });
    }

    void cancel() override
    {
        ref<Completer<T>> self = self();
        this->_callbackHandler->post([this, self] {
            if (!this->_isCompleted)
                this->_isCancelled = true;
        });
    }

protected:
    virtual void completeSync(const T &value)
    {
        assert(this->_isCompleted == false);
        if (this->_isCancelled)
            return;
        this->_future->_completed = true;
        this->_future->_data = std::move(value);
        for (auto &fn : this->_future->_callbackList)
            fn(this->_future->_data);
        this->_future->_callbackList->clear();
    }

    ref<Future<T>> _future;

public:
    const ref<Future<T>> &future = _future;
};