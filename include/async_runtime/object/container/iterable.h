#pragma once

#include "../function.h"
#include "../object.h"
#include "iterator.h"
#include "mixins.h"

template <typename T>
class Iterable : public virtual Object, public virtual ContainableMixin<T>
{
public:
    virtual bool any(Function<bool(const T &)>) const = 0;
    virtual bool every(Function<bool(const T &)>) const = 0;
    virtual void forEach(Function<void(const T &)>) const = 0;

    bool contain(const T &other) const override
    {
        return this->any([&](const T &value)
                         { return value == other; });
    }
};

template <typename T>
class MutableIterable : public virtual Iterable<T>,
                        public virtual SizeMixin,
                        public virtual AddableMixin<T>,
                        public virtual RemovableMixin<T>
{
public:
    virtual ref<ConstIterator<T>> begin() const { return const_cast<MutableIterable<T> *>(this)->begin()->toConst(); }
    virtual ref<ConstIterator<T>> end() const { return const_cast<MutableIterable<T> *>(this)->end()->toConst(); }
    virtual bool isEmpty() const { return this->begin() == this->end(); }
    virtual bool isNotEmpty() const { return !this->isEmpty(); }

    virtual ref<Iterator<T>> begin() = 0;
    virtual ref<Iterator<T>> end() = 0;

    virtual ref<Iterator<T>> erase(ref<Iterator<T>>) = 0;

    bool any(Function<bool(const T &)> fn) const override
    {
        for (const T &iter : *this)
            if (fn(iter))
                return true;
        return false;
    }

    bool every(Function<bool(const T &)> fn) const override
    {
        for (const T &iter : *this)
            if (!fn(iter))
                return false;
        return true;
    }

    void forEach(Function<void(const T &)> fn) const override
    {
        for (const T &iter : *this)
            fn(iter);
    }

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
