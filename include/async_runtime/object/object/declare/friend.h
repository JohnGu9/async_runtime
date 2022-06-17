#pragma once

#define _ASYNC_RUNTIME_FRIEND_FAMILY_WITHOUT_OBJECT \
    template <typename _TYPENAME_>                  \
    friend class ::_async_runtime::OptionImplement; \
    template <typename _TYPENAME_>                  \
    friend class ::option;                          \
    template <typename _TYPENAME_>                  \
    friend class ::_async_runtime::RefImplement;    \
    template <typename _TYPENAME_>                  \
    friend class ::ref;                             \
    template <typename _TYPENAME_>                  \
    friend class ::lateref;                         \
    template <typename _TYPENAME_>                  \
    friend class ::weakref;                         \
    template <typename _TYPENAME_>                  \
    friend class Future;                            \
    template <typename _TYPENAME_>                  \
    friend class Stream;                            \
    template <typename _TYPENAME_>                  \
    friend class FutureOr;

#define _ASYNC_RUNTIME_FRIEND_FAMILY            \
    _ASYNC_RUNTIME_FRIEND_FAMILY_WITHOUT_OBJECT \
    friend class Object;
