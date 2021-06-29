#pragma once

#include "../object.h"
#include "../basic/lock.h"
#include "../basic/duration.h"
#include "../widgets/stateful_widget.h"

#include "async/thread.h"
#include "async/thread_pool.h"

#include "state_helper.h"

#define _ASYNC_RUNTIME_FRIEND_ASYNC_FAMILY \
    template <typename R>                  \
    friend class Completer;                \
    template <typename R>                  \
    friend class Future;                   \
    template <typename R>                  \
    friend class Stream;                   \
    template <typename R>                  \
    friend class StreamController;         \
    template <typename R>                  \
    friend class StreamSubscription;       \
    template <typename R>                  \
    friend class AsyncSnapshot;

// @ thread safe
template <typename T = std::nullptr_t>
class Future;
// @ thread safe
template <typename T = std::nullptr_t>
class Completer;
// @ thread safe
template <typename T = std::nullptr_t>
class Stream;
// @ not thread safe
template <typename T = std::nullptr_t>
class StreamController;
// @ not thread safe
template <typename T = std::nullptr_t>
class StreamSubscription;
// @ thread safe
template <typename T = std::nullptr_t>
class AsyncSnapshot;

template <typename T>
ref<Future<T>> async(ref<ThreadPool> callbackHandler, Function<T()> fn);
template <typename T>
ref<Future<T>> async(ref<State<StatefulWidget>> state, Function<T()> fn);

#include "async/future.h"
#include "async/completer.h"
#include "async/stream_subscription.h"
#include "async/stream.h"
#include "async/stream_controller.h"
#include "async/async_snapshot.h"
#include "async/async.h"

/**
 * @brief Future<void> implement
 * 
 */

template <>
inline ref<Future<void>> Future<void>::than(Function<FutureOr<void>()> fn)
{
    ref<Future<void>> self = self();
    ref<Completer<void>> completer = Object::create<Completer<void>>(this->_callbackHandler);
    this->_callbackHandler->post([this, self, completer, fn]
                                 {
                                     Function<void()> handle = [completer, fn]
                                     {
                                         auto result = fn();
                                         lateref<Future<void>> future;
                                         if (result._future.isNotNull(future))
                                         {
                                             future->than([completer]
                                                          { completer->complete(); });
                                         }
                                         else
                                             completer->completeSync();
                                     };
                                     if (this->_completed == false)
                                         this->_callbackList->emplace_back(handle);
                                     else
                                         handle();
                                 });
    return completer->future;
}

template <typename ReturnType>
ref<Future<ReturnType>> Future<void>::than(Function<FutureOr<ReturnType>()> fn)
{
    ref<Future<void>> self = self();
    ref<Completer<ReturnType>> completer = Object::create<Completer<ReturnType>>(this->_callbackHandler);
    this->_callbackHandler->post([this, self, completer, fn]
                                 {
                                     Function<void()> handle = [completer, fn]
                                     {
                                         auto result = fn();
                                         lateref<Future<ReturnType>> future;
                                         if (result._future.isNotNull(future))
                                         {
                                             future->template than<void>([completer](const ReturnType &value) -> FutureOr<void>
                                                                {
                                                                    completer->complete(value);
                                                                    return {};
                                                                });
                                         }
                                         else
                                             completer->completeSync(result._value);
                                     };
                                     if (this->_completed == false)
                                         this->_callbackList->emplace_back(handle);
                                     else
                                         handle();
                                 });
    return completer->future;
}

/**
 * @brief Future<T> implement
 * 
 */

template <typename T>
ref<Future<T>> Future<T>::value(ref<ThreadPool> callbackHandler, const T &value)
{
    return Object::create<Future<T>>(callbackHandler, value);
}

template <typename T>
ref<Future<T>> Future<T>::value(ref<State<StatefulWidget>> state, const T &value)
{
    return Object::create<Future<T>>(StateHelper::getHandlerfromState(state), value);
}

template <typename T>
ref<Future<T>> Future<T>::value(ref<ThreadPool> callbackHandler, T &&value)
{
    return Object::create<Future<T>>(callbackHandler, value);
}

template <typename T>
ref<Future<T>> Future<T>::value(ref<State<StatefulWidget>> state, T &&value)
{
    return Object::create<Future<T>>(StateHelper::getHandlerfromState(state), value);
}

template <typename T>
template <typename ReturnType, typename std::enable_if<std::is_void<ReturnType>::value>::type *>
ref<Future<ReturnType>> Future<T>::than(Function<FutureOr<ReturnType>(const T &)> fn)
{
    ref<Future<T>> self = self();
    ref<Completer<void>> completer = Object::create<Completer<void>>(this->_callbackHandler);
    this->_callbackHandler->post([this, self, completer, fn]
                                 {
                                     Function<void(const T &)> handle = [completer, fn](const T &val)
                                     {
                                         auto result = fn(val);
                                         lateref<Future<ReturnType>> future;
                                         if (result._future.isNotNull(future))
                                         {
                                             future->than([completer]
                                                          { completer->complete(); });
                                         }
                                         else
                                             completer->completeSync();
                                     };
                                     if (this->_completed == false)
                                         this->_callbackList->emplace_back(handle);
                                     else
                                         handle(this->_data);
                                 });
    return completer->future;
}

template <typename T>
template <typename ReturnType, typename std::enable_if<!std::is_void<ReturnType>::value>::type *>
ref<Future<ReturnType>> Future<T>::than(Function<FutureOr<ReturnType>(const T &)> fn)
{
    ref<Future<T>> self = self();
    ref<Completer<ReturnType>> completer = Object::create<Completer<ReturnType>>(this->_callbackHandler);
    this->_callbackHandler->post([this, self, completer, fn]
                                 {
                                     Function<void(const T &)> handle = [completer, fn](const T &val)
                                     {
                                         auto result = fn(val);
                                         lateref<Future<ReturnType>> future;
                                         if (result._future.isNotNull(future))
                                         {
                                             future->template than<void>([completer](const ReturnType &value) -> FutureOr<void>
                                                                {
                                                                    completer->complete(value);
                                                                    return {};
                                                                });
                                         }
                                         else
                                             completer->completeSync(result._value);
                                     };
                                     if (this->_completed == false)
                                         this->_callbackList->emplace_back(handle);
                                     else
                                         handle(this->_data);
                                 });
    return completer->future;
}

template <typename T>
ref<Future<std::nullptr_t>> Future<T>::than(Function<void()> fn)
{
    ref<Future<T>> self = self();
    this->_callbackHandler->post([this, self, fn]
                                 {
                                     if (this->_completed == false)
                                         this->_callbackList->emplace_back([fn](const T &)
                                                                           { fn(); });
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
        ->than([=]
               {
                   if (completer->_isCompleted == false)
                       completer->completeSync(onTimeout());
               });
    this->than([=]
               {
                   if (completer->_isCompleted == false)
                       completer->completeSync(std::move(this->_data));
               });
    return completer->future;
}
