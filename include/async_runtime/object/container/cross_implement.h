#pragma once

#include "../container.h"

template <typename Key, typename Value>
ref<Iterable<Key>> Map<Key, Value>::keys() const
{
    return MapIterable<T, Key>::from(Object::cast<>(const_cast<Map<Key, Value> *>(this)),
                                     [](const T &pair) -> const Key &
                                     { return pair->first; });
}

template <typename Key, typename Value>
ref<Iterable<Value>> Map<Key, Value>::values() const
{
    return MapIterable<T, Value>::from(Object::cast<>(const_cast<Map<Key, Value> *>(this)),
                                       [](const T &pair) -> const Value &
                                       { return pair->second; });
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
