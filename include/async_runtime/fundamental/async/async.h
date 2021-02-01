#pragma once

#include "../async.h"

template <typename T>
Object::Ref<Future<T>> async(Object::Ref<ThreadPool> callbackHandler, Function<T()> fn);
template <typename T>
Object::Ref<Future<T>> async(State<StatefulWidget> *state, Function<T()> fn);

template <>
inline Object::Ref<Future<void>> async(Object::Ref<ThreadPool> callbackHandler, Function<void()> fn)
{
    Object::Ref<Future<void>> future = Object::create<Future<void>>(callbackHandler);
    future->_callbackHandler->post(fn);
    return future;
}

template <>
inline Object::Ref<Future<void>> async(State<StatefulWidget> *state, Function<void()> fn)
{
    Object::Ref<Future<void>> future = Object::create<Future<void>>(state);
    future->_callbackHandler->post(fn);
    return future;
}

template <typename T>
Object::Ref<Future<T>> async(Object::Ref<ThreadPool> callbackHandler, Function<T()> fn)
{
    Object::Ref<Completer<T>> completer = Object::create<Completer<T>>(callbackHandler);
    completer->_callbackHandler->post([fn, completer] { completer->completeSync(fn()); });
    return completer->future;
}

template <typename T>
Object::Ref<Future<T>> async(State<StatefulWidget> *state, Function<T()> fn)
{
    Object::Ref<Completer<T>> completer = Object::create<Completer<T>>(state);
    completer->_callbackHandler->post([fn, completer] { completer->completeSync(fn()); });
    return completer->future;
}