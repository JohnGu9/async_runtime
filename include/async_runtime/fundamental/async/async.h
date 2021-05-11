#pragma once

#include "../async.h"

template <typename T>
ref<Future<T>> async(ref<ThreadPool> callbackHandler, Function<T()> fn);
template <typename T>
ref<Future<T>> async(ref<State<StatefulWidget>> state, Function<T()> fn);

template <>
inline ref<Future<void>> async(ref<ThreadPool> callbackHandler, Function<void()> fn)
{
    ref<Completer<void>> completer = Object::create<Completer<void>>(callbackHandler);
    completer->_callbackHandler->post([fn, completer] { fn(); completer->completeSync(); });
    return completer->future;
}

template <>
inline ref<Future<void>> async(ref<State<StatefulWidget>> state, Function<void()> fn)
{
    ref<Completer<void>> completer = Object::create<Completer<void>>(state);
    completer->_callbackHandler->post([fn, completer] { fn(); completer->completeSync(); });
    return completer->future;
}

template <typename T>
option<Future<T>> async(ref<ThreadPool> callbackHandler, Function<T()> fn)
{
    ref<Completer<T>> completer = Object::create<Completer<T>>(callbackHandler);
    completer->_callbackHandler->post([fn, completer] { completer->completeSync(fn()); });
    return completer->future;
}

template <typename T>
ref<Future<T>> async(ref<State<StatefulWidget>> state, Function<T()> fn)
{
    ref<Completer<T>> completer = Object::create<Completer<T>>(state);
    completer->_callbackHandler->post([fn, completer] { completer->completeSync(fn()); });
    return completer->future;
}