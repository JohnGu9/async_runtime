#pragma once

#include "../async.h"

template <>
class Completer<std::nullptr_t> : public Object, StateHelper
{
    template <typename R>
    friend class Future;

    template <typename R>
    friend class Stream;

    template <typename R>
    friend class AsyncSnapshot;

protected:
    Completer(Object::Ref<ThreadPool> callbackHandler) : _callbackHandler(callbackHandler), _isCompleted(false), _isCancelled(false) {}
    Completer(State<StatefulWidget> *state) : _callbackHandler(getHandlerfromState(state)), _isCompleted(false), _isCancelled(false) {}

    std::atomic_bool _isCompleted;
    std::atomic_bool _isCancelled;
    Object::Ref<ThreadPool> _callbackHandler;

public:
    virtual void cancel() = 0;
    const std::atomic_bool &isCompleted = _isCompleted;
    const std::atomic_bool &isCancelled = _isCancelled;
};

template <>
class Completer<void> : public Completer<std::nullptr_t>
{
    template <typename R>
    friend class Future;

    template <typename R>
    friend class Stream;

    template <typename R>
    friend class AsyncSnapshot;

    template <typename R>
    friend Object::Ref<Future<R> > async(Object::Ref<ThreadPool> callbackHandler, Function<R()> fn);

    template <typename R>
    friend Object::Ref<Future<R> > async(State<StatefulWidget> *state, Function<R()> fn);

public:
    Completer(Object::Ref<ThreadPool> callbackHandler) : Completer<std::nullptr_t>(callbackHandler), _future(Object::create<Future<void> >(callbackHandler)) {}
    Completer(State<StatefulWidget> *state) : Completer<std::nullptr_t>(state), _future(Object::create<Future<void> >(state)) {}

    virtual void complete()
    {
        Object::Ref<Completer<void> > self = Object::cast<>(this);
        this->_callbackHandler->post([self] { self->completeSync(); });
    }

    void cancel() override
    {
        Object::Ref<Completer<void> > self = Object::cast<>(this);
        this->_callbackHandler->post([self] {
            if (self->_isCompleted)
                return false; // already completed
            self->_isCancelled = true;
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
        this->_future->_callbackList = nullptr;
    }

    Object::Ref<Future<void> > _future;

public:
    const Object::Ref<Future<void> > &future = _future;
};

template <typename T>
class Completer : public Completer<std::nullptr_t>
{
    template <typename R>
    friend class Future;

    template <typename R>
    friend class Stream;

    template <typename R>
    friend class AsyncSnapshot;

    template <typename R>
    friend Object::Ref<Future<R> > async(Object::Ref<ThreadPool> callbackHandler, Function<R()> fn);

    template <typename R>
    friend Object::Ref<Future<R> > async(State<StatefulWidget> *state, Function<R()> fn);

public:
    Completer(Object::Ref<ThreadPool> callbackHandler) : Completer<std::nullptr_t>(callbackHandler), _future(Object::create<Future<T> >(callbackHandler)) {}
    Completer(State<StatefulWidget> *state) : Completer<std::nullptr_t>(state), _future(Object::create<Future<T> >(state)) {}

    virtual void complete(const T &value)
    {
        Object::Ref<Completer<T> > self = Object::cast<>(this);
        this->_callbackHandler->post([self](const T &value) { self->completeSync(value); }, std::move(value));
    }

    virtual void complete(T &&value)
    {
        Object::Ref<Completer<T> > self = Object::cast<>(this);
        this->_callbackHandler->post([self](const T &value) { self->completeSync(value); }, std::move(value));
    }

    void cancel() override
    {
        Object::Ref<Completer<T> > self = Object::cast<>(this);
        this->_callbackHandler->post([self] {
            if (!self->_isCompleted)
                self->_isCancelled = true;
        });
    }

protected:
    virtual void completeSync(const T &value)
    {
        assert(this->_isCompleted == false);
        if (this->_isCancelled)
            return;
        this->_future->_data = std::move(value);
        this->_future->_completed = true;
        for (auto &fn : this->_future->_callbackList)
            fn(this->_future->_data);
        this->_future->_callbackList = nullptr;
    }

    Object::Ref<Future<T> > _future;

public:
    const Object::Ref<Future<T> > &future = _future;
};