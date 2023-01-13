#pragma once
#include "../object/object.h"
#include "../utilities/duration.h"
#include "event_loop.h"

// Forward define
// All async components are not thread safe
// You need to handle multi-threading issue by your own
// You can checkout [utilities/lock.h] for some ease thread-locker

template <typename T = Object::Void>
class Future;
template <typename T = Object::Void>
class Completer;
template <typename T = Object::Void>
class Stream;
template <typename T = Object::Void>
class StreamController;
template <typename T = Object::Void>
class StreamSubscription;
template <typename T = Object::Void>
class AsyncSnapshot;

#define _ASYNC_RUNTIME_FRIEND_ASYNC_FAMILY \
    template <typename>                    \
    friend class Future;                   \
    template <typename>                    \
    friend class Completer;                \
    template <typename>                    \
    friend class Stream;                   \
    template <typename>                    \
    friend class StreamController;         \
    template <typename>                    \
    friend class StreamSubscription;       \
    template <typename>                    \
    friend class AsyncSnapshot;

#include "async/async_snapshot.h"
#include "async/future.h"
#include "async/stream.h"
#include "async/stream_subscription.h"

#include "timer.h"

template <typename T>
ref<Future<T>> Future<T>::delay(Duration timeout, Function<T()> fn, option<EventLoopGetterMixin> getter)
{
    auto future = Object::create<Future<T>>(getter);
    Timer::delay(
        timeout,
        [future, fn](ref<Timer> timer) { //
            future->_data = fn();
            future->markAsCompleted();
        },
        getter)
        ->start();
    return future;
}

template <typename T>
ref<Future<T>> Future<T>::delay(Duration timeout, T value, option<EventLoopGetterMixin> getter)
{
    auto future = Object::create<Future<T>>(getter);
    future->_data = std::move(value);
    Timer::delay(
        timeout,
        [future](ref<Timer> timer) { //
            future->markAsCompleted();
        },
        getter)
        ->start();
    return future;
}

template <typename T>
ref<Future<T>> Future<T>::timeout(Duration timeout, Function<T()> onTimeout)
{
    ref<Future<T>> self = self();
    ref<Future<T>> future = Object::create<Future<T>>(self);
    this->template then<int>([future](const T &value) { //
        if (!future->completed())
        {
            future->_data = value;
            future->markAsCompleted();
        }
        return 0;
    });
    Timer::delay(
        timeout,
        [future, onTimeout](ref<Timer> timer) { //
            if (!future->completed())
            {
                future->_data = onTimeout();
                future->markAsCompleted();
            }
        },
        self)
        ->start();
    return future;
}
