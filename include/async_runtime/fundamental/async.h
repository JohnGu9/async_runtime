#pragma once

#include "../object.h"
#include "../basic/lock.h"
#include "../basic/duration.h"

#include "event_loop.h"

#define _ASYNC_RUNTIME_FRIEND_ASYNC_FAMILY \
    template <typename R>                  \
    friend class Future;                   \
    template <typename R>                  \
    friend class Completer;        \
    template <typename R>                  \
    friend class Stream;                   \
    template <typename R>                  \
    friend class StreamController;         \
    template <typename R>                  \
    friend class StreamSubscription;       \
    template <typename R>                  \
    friend class AsyncSnapshot;

// Forward define

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

#include "async/future.h"
#include "async/stream.h"
#include "async/stream_subscription.h"
#include "async/async_snapshot.h"

template <typename T>
ref<Future<T>> async(Function<T()> fn, option<EventLoopGetterMixin> getter = nullptr)
{
    auto loop = EventLoopGetterMixin::ensureEventLoop(getter);
    auto future = Object::create<Completer<T>>(getter);
    loop->callSoon([future, fn]
                   { future->resolve(fn()); });
    return future;
}

#include "timer.h"

template <typename T>
ref<Future<T>> delay(Duration timeout, Function<T()> fn, option<EventLoopGetterMixin> getter = nullptr)
{
    auto future = Object::create<Future<T>>(getter);
    auto timer = Timer::delay(
        timeout, [future, fn](ref<Timer> timer)
        { future->resolve(fn()); },
        getter);
    return future;
}

template <typename T>
ref<Future<T>> Future<T>::timeout(Duration timeout, Function<T()> onTimeout)
{
    ref<Future<T>> self = self();
    ref<Completer<T>> future = Object::create<Completer<T>>(self);
    self->template than<int>([future](const T &value)
                             {if(!future->completed()) future->resolve(value);
               return 0; });
    Timer::delay(
        timeout, [future, onTimeout](ref<Timer> timer)
        { if(!future->completed()) future->resolve(onTimeout()); },
        self);
    return future;
}