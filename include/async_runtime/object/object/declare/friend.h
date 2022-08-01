#pragma once

#define _ASYNC_RUNTIME_FRIEND_FAMILY_WITHOUT_OBJECT \
    template <typename>                             \
    friend class ::_async_runtime::OptionImplement; \
    template <typename>                             \
    friend class ::option;                          \
    template <typename>                             \
    friend class ::_async_runtime::RefImplement;    \
    template <typename>                             \
    friend class ::ref;                             \
    template <typename>                             \
    friend class ::lateref;                         \
    template <typename>                             \
    friend class ::weakref;                         \
    template <typename>                             \
    friend class Future;                            \
    template <typename>                             \
    friend class Stream;                            \
    template <typename>                             \
    friend class FutureOr;

#define _ASYNC_RUNTIME_FRIEND_FAMILY            \
    _ASYNC_RUNTIME_FRIEND_FAMILY_WITHOUT_OBJECT \
    friend class Object;
