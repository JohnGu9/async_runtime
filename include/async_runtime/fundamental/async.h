#pragma once

#include "../object.h"

// @ thread safe
template <typename T = std::nullptr_t>
class Future;
// @ thread safe
template <typename T = std::nullptr_t>
class Completer;
// @ thread safe
template <typename T = std::nullptr_t>
class Stream;
// @ thread safe
template <typename T = std::nullptr_t>
class StreamSubscription;
// @ thread safe
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
#include "async/stream_subscription.h"
#include "async/stream.h"
#include "async/async_snapshot.h"
#include "async/async.h"

////////////////////////////
//
// Future<void> implement
//
////////////////////////////

template <>
inline ref<Future<void>> Future<void>::than(Function<void()> fn)
{
    ref<Future<void>> self = self();
    ref<Completer<void>> completer = Object::create<Completer<void>>(this->_callbackHandler);
    this->_callbackHandler->post([self, completer, fn] {
        if (self->_completed == false)
            self->_callbackList->emplace_back([completer, fn] { fn(); completer->completeSync(); });
        else
        {
            fn();
            completer->completeSync();
        }
    });
    return completer->future;
}

template <typename ReturnType>
ref<Future<ReturnType>> Future<void>::than(Function<ReturnType()> fn)
{
    ref<Future<void>> self = self();
    ref<Completer<ReturnType>> completer = Object::create<Completer<ReturnType>>(this->_callbackHandler);
    this->_callbackHandler->post([self, completer, fn] {
        if (self->_completed == false)
            self->_callbackList->emplace_back([completer, fn] { completer->completeSync(fn()); });
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
ref<Future<T>> Future<T>::value(ref<ThreadPool> callbackHandler, const T &value)
{
    return Object::create<Future<T>>(callbackHandler, value);
}

template <typename T>
ref<Future<T>> Future<T>::value(State<StatefulWidget> *state, const T &value)
{
    return Object::create<Future<T>>(state, value);
}

template <typename T>
ref<Future<T>> Future<T>::value(ref<ThreadPool> callbackHandler, T &&value)
{
    return Object::create<Future<T>>(callbackHandler, value);
}

template <typename T>
ref<Future<T>> Future<T>::value(State<StatefulWidget> *state, T &&value)
{
    return Object::create<Future<T>>(state, value);
}

template <typename T>
template <typename ReturnType, typename std::enable_if<std::is_void<ReturnType>::value>::type *>
ref<Future<ReturnType>> Future<T>::than(Function<ReturnType(const T &)> fn)
{
    ref<Completer<void>> completer = Object::create<Completer<void>>(this->_callbackHandler);
    ref<Future<T>> self = self();
    this->_callbackHandler->post([self, completer, fn] {
        if (self->_completed == false)
            self->_callbackList->emplace_back([completer, fn](const T &value) { fn(value); completer->completeSync(); });
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
ref<Future<ReturnType>> Future<T>::than(Function<ReturnType(const T &)> fn)
{
    ref<Completer<ReturnType>> completer = Object::create<Completer<ReturnType>>(this->_callbackHandler);
    ref<Future<T>> self = self();
    this->_callbackHandler->post([self, completer, fn] {
        if (self->_completed == false)
            self->_callbackList->emplace_back([completer, fn](const T &value) { completer->completeSync(fn(value)); });
        else
            completer->completeSync(fn(self->_data));
    });
    return completer->future;
}

template <typename T>
ref<Future<std::nullptr_t>> Future<T>::than(Function<void()> fn)
{
    ref<Future<T>> self = self();
    this->_callbackHandler->post([self, fn] {
        if (self->_completed == false)
            self->_callbackList->emplace_back([fn](const T &) { fn(); });
        else
            fn();
    });
    return self;
}

template <typename T>
ref<Future<T>> Future<T>::timeout(Duration duration, Function<T()> onTimeout)
{
    ref<Future<T>> self = self();
    ref<Completer<T>> completer = Object::create<Completer<T>>(this->_callbackHandler);
    Future<void>::delay(this->_callbackHandler, duration)
        ->than([=] {
            if (completer->_isCompleted == false)
                completer->completeSync(onTimeout());
        });
    this->than([=] {
        if (completer->_isCompleted == false)
            completer->completeSync(std::move(this->_data));
    });
    return completer->future;
}

////////////////////////////
//
// Completer implement
//
////////////////////////////
