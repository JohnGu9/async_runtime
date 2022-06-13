#pragma once

#include "../container.h"

template <typename Key, typename Value>
ref<Iterable<Key>> Map<Key, Value>::keys() const
{
    auto list = List<Key>::create();
    for (const T &pair : *this)
        list->emplace(pair.first);
    return list;
}

template <typename Key, typename Value>
ref<Iterable<Value>> Map<Key, Value>::values() const
{
    auto list = List<Value>::create();
    for (const T &pair : *this)
        list->emplace(pair.second);
    return list;
}

template <typename T>
ref<Set<T>> List<T>::toSet() const
{
    auto set = Set<T>::create();
    for (const T &element : *this)
        set->emplace(element);
    return set;
}

template <typename T>
ref<List<T>> Set<T>::toList() const
{
    auto list = List<T>::create();
    for (const T &element : *this)
        list->emplace(element);
    return list;
}