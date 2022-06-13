#pragma once

#include "iterator.h"

template <typename T>
class AddableMixin
{
public:
    virtual bool add(const T &) = 0;
    virtual bool add(T &&v) { return this->add(static_cast<const T &>(v)); }

    virtual void emplace(const T &v) { this->add(v); }
    virtual void emplace(T &&v) { this->add(std::move(v)); }
};

template <typename T>
class RemovableMixin
{
public:
    virtual bool remove(const T &) = 0;
    virtual bool remove(T &&v) { return this->remove(static_cast<const T &>(v)); }

    virtual bool removeAll(const T &v) { return this->remove(v); }
    virtual bool removeAll(T &&v) { return this->removeAll(static_cast<const T &>(v)); }
};

template <typename Key, typename Value>
class ConstIndexableMixin
{
public:
    virtual const Value &operator[](const Key &) const = 0;
    virtual const Value &operator[](Key &&key) const { return this->operator[](static_cast<const Key &>(key)); }
};

template <typename Key, typename Value>
class IndexableMixin : public ConstIndexableMixin<Key, Value>
{
public:
    virtual Value &operator[](const Key &) = 0;
    virtual Value &operator[](Key &&key) { return this->operator[](static_cast<const Key &>(key)); }

    const Value &operator[](const Key &key) const override { return const_cast<IndexableMixin *>(this)->operator[](key); }
};
