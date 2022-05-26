#pragma once

#define _ASYNC_RUNTIME_FRIEND_FAMILY                \
    template <typename R>                           \
    friend class ::_async_runtime::OptionImplement; \
    template <typename R>                           \
    friend class ::option;                          \
    template <typename R>                           \
    friend class ::_async_runtime::RefImplement;    \
    template <typename R>                           \
    friend class ::ref;                             \
    template <typename R>                           \
    friend class ::lateref;                         \
    template <typename R>                           \
    friend class ::weakref;                         \
    friend class Object;                            \
    template <typename R>                           \
    friend class Future;                            \
    template <typename R>                           \
    friend class Stream;                            \
    template <typename R>                           \
    friend class FutureOr;
