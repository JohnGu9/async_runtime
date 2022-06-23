#pragma once

#include "../container.h"

template <typename T>
ref<Iterable<T>>::ref(const std::initializer_list<T> &list) : super(Object::create<_async_runtime::DefaultList<T>>(list)) {}

template <typename T>
ref<Iterable<T>>::ref(std::initializer_list<T> &&list) : super(Object::create<_async_runtime::DefaultList<T>>(std::move(list))) {}

template <typename Key, typename Value>
ref<Iterable<Key>> Map<Key, Value>::keys() const
{
    return MapIterable<Pair, Key>::from(Object::cast<>(const_cast<Map<Key, Value> *>(this)),
                                     [](const Pair &pair) -> const Key &
                                     { return pair->first; });
}

template <typename Key, typename Value>
ref<Iterable<Value>> Map<Key, Value>::values() const
{
    return MapIterable<Pair, Value>::from(Object::cast<>(const_cast<Map<Key, Value> *>(this)),
                                       [](const Pair &pair) -> const Value &
                                       { return pair->second; });
}

template <typename T>
ref<Set<T>> Iterable<T>::toSet() const
{
    auto set = Set<T>::create();
    for (const T &element : *this)
        set->emplace(element);
    return set;
}

template <typename T>
ref<List<T>> Iterable<T>::toList() const
{
    auto list = List<T>::create();
    for (const T &element : *this)
        list->emplace(element);
    return list;
}
