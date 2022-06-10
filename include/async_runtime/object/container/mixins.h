#pragma once

template <typename T>
class ContainableMixin
{
public:
    virtual bool contain(const T &other) const = 0;
    virtual bool contain(T &&other) const { return this->contain(static_cast<const T &>(other)); }
};

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

class SizeMixin
{
public:
    virtual size_t size() const = 0;
};

template <typename Key, typename Value>
class IndexableMixin
{
public:
    virtual Value &operator[](const Key &) = 0;
    virtual Value &operator[](Key &&key) { return this->operator[](static_cast<const Key &>(key)); }
};

template <typename T>
class InsertMixin
{
public:
    virtual bool insert(size_t index, const T &value) { throw NotImplementedError(); }
    virtual bool insert(size_t index, T &&value) { return this->insert(index, static_cast<const T &>(value)); }
};
