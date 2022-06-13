#pragma once

#include "../object.h"
#include "iterator.h"
#include "mixins.h"

template <typename T>
class Iterable : public virtual Object
{
public:
    virtual size_t size() const = 0;
    virtual ref<ConstIterator<T>> begin() const = 0;
    virtual ref<ConstIterator<T>> end() const = 0;

    virtual bool isEmpty() const { return this->size() == 0; }
    virtual bool isNotEmpty() const { return !this->isEmpty(); }

    virtual bool any(Function<bool(const T &)> fn) const
    {
        for (const T &iter : *this)
            if (fn(iter))
                return true;
        return false;
    }

    virtual bool every(Function<bool(const T &)> fn) const
    {
        for (const T &iter : *this)
            if (!fn(iter))
                return false;
        return true;
    }

    virtual void forEach(Function<void(const T &)> fn) const
    {
        for (const T &iter : *this)
            fn(iter);
    }

    virtual bool contain(const T &other) const
    {
        return this->any([&](const T &value)
                         { return value == other; });
    }

    virtual bool contain(T &&other) const
    {
        return this->contain(static_cast<const T &>(other));
    }
};

template <typename T>
class MutableIterable : public virtual Iterable<T>,
                        public virtual AddableMixin<T>,
                        public virtual RemovableMixin<T>
{
public:
    ref<ConstIterator<T>> begin() const override { return const_cast<MutableIterable<T> *>(this)->begin()->toConst(); }
    ref<ConstIterator<T>> end() const override { return const_cast<MutableIterable<T> *>(this)->end()->toConst(); }

    virtual ref<Iterator<T>> begin() = 0;
    virtual ref<Iterator<T>> end() = 0;
    virtual ref<Iterator<T>> erase(ref<Iterator<T>>) = 0;

    bool remove(const T &value) override
    {
        for (auto it = this->begin(); it != this->end(); ++it)
        {
            if (*it == value)
            {
                this->erase(it);
                return true;
            }
        }
        return false;
    }

    bool removeAll(const T &value) override
    {
        for (auto it = this->begin(); it != this->end(); ++it)
        {
            if (*it == value)
                it = this->erase(it);
        }
        return true;
    }

    virtual void clear() = 0;
};
