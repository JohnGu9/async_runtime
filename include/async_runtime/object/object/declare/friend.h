#pragma once

template <typename First, typename Second>
class Pair; // msvc need to forward declare before be someone's friend class

#define _ASYNC_RUNTIME_FRIEND_FAMILY_WITHOUT_OBJECT       \
    template <typename _TYPENAME_>                        \
    friend class ::_async_runtime::OptionImplement;       \
    template <typename _TYPENAME_>                        \
    friend class ::option;                                \
    template <typename _TYPENAME_>                        \
    friend class ::_async_runtime::RefImplement;          \
    template <typename _TYPENAME_>                        \
    friend class ::ref;                                   \
    template <typename _TYPENAME_>                        \
    friend class ::lateref;                               \
    template <typename _TYPENAME_>                        \
    friend class ::weakref;                               \
    template <typename _TYPENAME_>                        \
    friend class Future;                                  \
    template <typename _TYPENAME_>                        \
    friend class Stream;                                  \
    template <typename _TYPENAME_>                        \
    friend class FutureOr;                                \
    template <typename _TYPENAME0_, typename _TYPENAME1_> \
    friend class Pair;

#define _ASYNC_RUNTIME_FRIEND_FAMILY            \
    _ASYNC_RUNTIME_FRIEND_FAMILY_WITHOUT_OBJECT \
    friend class Object;
