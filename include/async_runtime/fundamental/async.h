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
// Future<void> implement
//
////////////////////////////

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

////////////////////////////
//
// Future<T> implement
//
////////////////////////////

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
Object::Ref<Future<std::nullptr_t>> Future<T>::than(Function<void()> fn)
{
    Object::Ref<Future<T>> self = Object::cast<>(this);
    this->_callbackHandler->post([self, fn] {
        if (self->_completed == false)
            self->_callbackList.push_back([fn](const T &) { fn(); });
        else
            fn();
    });
    return self;
}

template <typename T>
Object::Ref<Future<T>> Future<T>::timeout(Duration, Function<void()> onTimeout)
{
    //TODO: implement function
    return Object::cast<>(this);
}

////////////////////////////
//
// Completer implement
//
////////////////////////////
