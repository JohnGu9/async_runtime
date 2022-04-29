#pragma once

#include "../basic/duration.h"
#include "../object.h"

#include "event_loop.h"

// Forward define
// All async components are not thread safe
// You need to handle multi-threading issue by your own
// You can checkout [basic/lock.h] for some ease thread-locker

template <typename T = std::nullptr_t>
class Future;
template <typename T = std::nullptr_t>
class Completer;
template <typename T = std::nullptr_t>
class Stream;
template <typename T = std::nullptr_t>
class StreamController;
template <typename T = std::nullptr_t>
class StreamSubscription;
template <typename T = std::nullptr_t>
class AsyncSnapshot;

#define _ASYNC_RUNTIME_FRIEND_ASYNC_FAMILY \
    template <typename R>                  \
    friend class Future;                   \
    template <typename R>                  \
    friend class Completer;                \
    template <typename R>                  \
    friend class Stream;                   \
    template <typename R>                  \
    friend class StreamController;         \
    template <typename R>                  \
    friend class StreamSubscription;       \
    template <typename R>                  \
    friend class AsyncSnapshot;

#include "async/async_snapshot.h"
#include "async/future.h"
#include "async/stream.h"
#include "async/stream_subscription.h"

#include "timer.h"

template <typename T>
ref<Future<T>> Future<T>::delay(Duration timeout, Function<T()> fn, option<EventLoopGetterMixin> getter)
{
    auto future = Object::create<Completer<T>>(getter);
    Timer::delay(
        timeout, [future, fn](ref<Timer> timer)
        { future->complete(fn()); },
        getter)
        ->start();
    return future;
}

template <typename T>
ref<Future<T>> Future<T>::delay(Duration timeout, T value, option<EventLoopGetterMixin> getter)
{
    auto future = Object::create<Completer<T>>(getter);
    Timer::delay(
        timeout, [future, value](ref<Timer> timer)
        { future->complete(value); },
        getter)
        ->start();
    return future;
}

template <typename T>
ref<Future<T>> Future<T>::timeout(Duration timeout, Function<T()> onTimeout)
{
    ref<Future<T>> self = self();
    ref<Completer<T>> future = Object::create<Completer<T>>(self);
    self->template then<int>([future](const T &value)
                             {if(!future->completed()) future->complete(value);
               return 0; });
    Timer::delay(
        timeout, [future, onTimeout](ref<Timer> timer)
        { if(!future->completed()) future->complete(onTimeout()); },
        self)
        ->start();
    return future;
}