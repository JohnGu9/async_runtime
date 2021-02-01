#pragma once

#include "../object.h"

// @ thread safe
template <typename T = std::nullptr_t>
class Future;
template <typename T = std::nullptr_t>
class Stream;
template <typename T = std::nullptr_t>
class Completer;
template <typename T = std::nullptr_t>
class AsyncSnapshot;

class ThreadPool;

#include "async/thread.h"
#include "async/thread_pool.h"

#include "../basic/function.h"
#include "../basic/duration.h"
#include "../widgets/state.h"
#include "state_helper.h"

#include "async/future.h"
#include "async/completer.h"
#include "async/stream.h"
#include "async/async_snapshot.h"
#include "async/async.h"

////////////////////////////
//
// Future implement
//
////////////////////////////

inline Object::Ref<Future<void>> Future<std::nullptr_t>::race(Set<Object::Ref<Future<>>> &&set)
{
    //TODO: implement function
    assert(!set.empty());
    Object::Ref<Completer<void>> completer = Object::create<Completer<void>>((*set.begin())->_callbackHandler);
    return completer->future;
}

inline Object::Ref<Future<void>> Future<std::nullptr_t>::wait(Set<Object::Ref<Future<>>> &&set)
{
    //TODO: implement function
    assert(!set.empty());
    Object::Ref<Completer<void>> completer = Object::create<Completer<void>>((*set.begin())->_callbackHandler);
    return completer->future;
}

inline Object::Ref<Future<void>> Future<void>::value(Object::Ref<ThreadPool> callbackHandler)
{
    Object::Ref<Completer<void>> completer = Object::create<Completer<void>>(callbackHandler);
    completer->complete();
    return completer->future;
}

inline Object::Ref<Future<void>> Future<void>::value(State<StatefulWidget> *state)
{
    Object::Ref<Completer<void>> completer = Object::create<Completer<void>>(state);
    completer->complete();
    return completer->future;
}

template <typename T>
Object::Ref<Future<T>> Future<T>::value(Object::Ref<ThreadPool> callbackHandler, const T &value)
{
    return Object::create<Future<T>>(callbackHandler, value);
}

template <typename T>
Object::Ref<Future<T>> Future<T>::value(State<StatefulWidget> *state, const T &value)
{
    return Object::create<Future<T>>(state, value);
}

template <typename T>
Object::Ref<Future<T>> Future<T>::value(Object::Ref<ThreadPool> callbackHandler, T &&value)
{
    return Object::create<Future<T>>(callbackHandler, value);
}

template <typename T>
Object::Ref<Future<T>> Future<T>::value(State<StatefulWidget> *state, T &&value)
{
    return Object::create<Future<T>>(state, value);
}

template <>
inline Object::Ref<Future<void>> Future<void>::than(Function<void()> fn)
{
    Object::Ref<Future<void>> self = Object::cast<>(this);
    Object::Ref<Completer<void>> completer = Object::create<Completer<void>>(this->_callbackHandler);
    this->_callbackHandler->post([self, completer, fn] {
        if (self->_completed == false)
            self->_callbackList.push_back([completer, fn] { fn(); completer->completeSync(); });
        else
        {
            fn();
            completer->completeSync();
        }
    });
    return completer->future;
}

template <typename ReturnType>
Object::Ref<Future<ReturnType>> Future<void>::than(Function<ReturnType()> fn)
{
    Object::Ref<Future<void>> self = Object::cast<>(this);
    Object::Ref<Completer<ReturnType>> completer = Object::create<Completer<ReturnType>>(this->_callbackHandler);
    this->_callbackHandler->post([self, completer, fn] {
        if (self->_completed == false)
            self->_callbackList.push_back([completer, fn] { completer->completeSync(fn()); });
        else
            completer->completeSync(fn());
    });
    return completer->future;
}

inline Object::Ref<Future<void>> Future<void>::than(Function<void()> fn)
{
    Object::Ref<Future<void>> self = Object::cast<>(this);
    this->_callbackHandler->post([self, fn] {
        if (self->_completed == false)
            self->_callbackList.push_back(fn);
        else
            fn();
    });

    return self;
}

inline Object::Ref<Future<void>> Future<void>::timeout(Duration, Function<void()> onTimeout)
{
    //TODO: implement function
    return Object::cast<>(this);
}

inline void Future<void>::sync()
{
    std::mutex mutex;
    std::condition_variable condition;
    std::unique_lock<std::mutex> lock(mutex);
    this->than<void>([&] { condition.notify_all(); });
    condition.wait(lock);
}

inline void Future<void>::sync(Duration timeout)
{
    std::mutex mutex;
    std::condition_variable condition;
    std::unique_lock<std::mutex> lock(mutex);
    this->than<void>([&] { condition.notify_all(); });
    condition.wait(lock);
}

template <typename T>
template <typename ReturnType, typename std::enable_if<std::is_void<ReturnType>::value>::type *>
Object::Ref<Future<ReturnType>> Future<T>::than(Function<ReturnType(const T &)> fn)
{
    Object::Ref<Completer<void>> completer = Object::create<Completer<void>>(this->_callbackHandler);
    Object::Ref<Future<T>> self = Object::cast<>(this);
    this->_callbackHandler->post([self, completer, fn] {
        if (self->_completed == false)
            self->_callbackList.push_back([completer, fn](const T &value) { fn(value); completer->completeSync(); });
        else
        {
            fn(self->_data);
            completer->completeSync();
        }
    });
    return completer->future;
}

template <typename T>
template <typename ReturnType, typename std::enable_if<!std::is_void<ReturnType>::value>::type *>
Object::Ref<Future<ReturnType>> Future<T>::than(Function<ReturnType(const T &)> fn)
{
    Object::Ref<Completer<ReturnType>> completer = Object::create<Completer<ReturnType>>(this->_callbackHandler);
    Object::Ref<Future<T>> self = Object::cast<>(this);
    this->_callbackHandler->post([self, completer, fn] {
        if (self->_completed == false)
            self->_callbackList.push_back([completer, fn](const T &value) { completer->completeSync(fn(value)); });
        else
            completer->completeSync(fn(self->_data));
    });
    return completer->future;
}

template <typename T>
Object::Ref<Future<void>> Future<T>::than(Function<void()> fn)
{
    return this->than<void>([fn](const T &) { fn(); });
}

template <typename T>
Object::Ref<Future<T>> Future<T>::timeout(Duration, Function<void()> onTimeout)
{
    //TODO: implement function
    return Object::cast<>(this);
}

template <typename T>
void Future<T>::sync()
{
    std::mutex mutex;
    std::condition_variable condition;
    std::unique_lock<std::mutex> lock(mutex);
    this->than<void>([&](const T &) { condition.notify_all(); });
    condition.wait(lock);
}

template <typename T>
void Future<T>::sync(Duration timeout)
{
    std::mutex mutex;
    std::condition_variable condition;
    std::unique_lock<std::mutex> lock(mutex);
    this->than<void>([&](const T &) { condition.notify_all(); });
    condition.wait(lock);
}

////////////////////////////
//
// Completer implement
//
////////////////////////////
