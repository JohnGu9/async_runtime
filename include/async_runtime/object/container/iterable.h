#pragma once

#include "../object.h"
#include "declare.h"
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
        for (const T &element : *this)
            if (fn(element))
                return true;
        return false;
    }

    virtual bool every(Function<bool(const T &)> fn) const
    {
        for (const T &element : *this)
            if (!fn(element))
                return false;
        return true;
    }

    virtual void forEach(Function<void(const T &)> fn) const
    {
        for (const T &element : *this)
            fn(element);
    }

    virtual ref<ConstIterator<T>> find(const T &value) const
    {
        auto end = this->end(),
             it = this->begin();
        for (; it != end; ++it)
            if (*it == value)
                break;
        return it;
    }

    virtual ref<ConstIterator<T>> find(T &&value) const
    {
        return this->find(static_cast<const T &>(value));
    }

    virtual bool contains(const T &other) const
    {
        return this->find(other) != this->end();
    }

    virtual bool contains(T &&other) const
    {
        return this->contains(static_cast<const T &>(other));
    }

    void toStringStream(std::ostream &os) override
    {
        os << typeid(*this).name() << "[ ";
        for (const T &element : *this)
            os << element << ", ";
        os << "]";
    }

    virtual ref<Set<T>> toSet() const;
    virtual ref<List<T>> toList() const;
};

template <typename T>
class MutableIterable : public virtual Iterable<T>,
                        public virtual EmplaceMixin<T>,
                        public virtual RemovableMixin<T>
{
public:
    ref<ConstIterator<T>> begin() const override { return const_cast<MutableIterable<T> *>(this)->begin().get()->toConst(); }
    ref<ConstIterator<T>> end() const override { return const_cast<MutableIterable<T> *>(this)->end().get()->toConst(); }

    virtual ref<Iterator<T>> begin() = 0;
    virtual ref<Iterator<T>> end() = 0;
    virtual ref<Iterator<T>> erase(ref<Iterator<T>>) = 0;

    virtual ref<Iterator<T>> find(const T &value)
    {
        auto end = this->end(),
             it = this->begin();
        for (; it != end; ++it)
            if (*it == value)
                break;
        return it;
    }

    virtual ref<Iterator<T>> find(T &&value)
    {
        return this->find(static_cast<const T &>(value));
    }

    bool remove(const T &value) override
    {
        auto it = this->find(value);
        if (it != this->end())
        {
            this->erase(it);
            return true;
        }
        return false;
    }

    bool removeAll(const T &value) override
    {
        for (auto it = this->begin(), end = this->end(); it != end;)
        {
            if (*it == value)
                it = this->erase(it);
            else
                ++it;
        }
        return true;
    }

    virtual void clear() = 0;
};

#include "ref.h"
