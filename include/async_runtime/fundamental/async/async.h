#pragma once

#include "../async.h"

template <typename T>
ref<Future<T>> async(ref<ThreadPool> callbackHandler, Function<T()> fn);
template <typename T>
ref<Future<T>> async(State<StatefulWidget> *state, Function<T()> fn);

template <>
inline ref<Future<void>> async(ref<ThreadPool> callbackHandler, Function<void()> fn)
{
    ref<Future<void>> future = Object::create<Future<void>>(callbackHandler);
    future->_callbackHandler->post(fn);
    return future;
}

template <>
inline ref<Future<void>> async(State<StatefulWidget> *state, Function<void()> fn)
{
    ref<Future<void>> future = Object::create<Future<void>>(state);
    future->_callbackHandler->post(fn);
    return future;
}

template <typename T>
ref<Future<T>> async(ref<ThreadPool> callbackHandler, Function<T()> fn)
{
    ref<Completer<T>> completer = Object::create<Completer<T>>(callbackHandler);
    completer->_callbackHandler->post([fn, completer] { completer->completeSync(fn()); });
    return completer->future;
}

template <typename T>
ref<Future<T>> async(State<StatefulWidget> *state, Function<T()> fn)
{
    ref<Completer<T>> completer = Object::create<Completer<T>>(state);
    completer->_callbackHandler->post([fn, completer] { completer->completeSync(fn()); });
    return completer->future;
}