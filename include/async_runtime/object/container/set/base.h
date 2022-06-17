#pragma once

#include "../list.h"

template <typename T>
class Set : public Iterable<T>,
            public EmplaceMixin<T>,
            public RemovableMixin<T>
{
    _ASYNC_RUNTIME_FRIEND_FAMILY;

protected:
    Set() {}

public:
    static ref<Set<T>> create();
    virtual ref<Set<T>> copy() const = 0;

    virtual ref<ConstIterator<T>> erase(ref<ConstIterator<T>> iter) = 0;
    virtual void clear() = 0;

    virtual void insert(const T &element) { this->emplace(element); }
    virtual void insert(T &&element) { this->emplace(std::move(element)); }

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

    ref<Set<T>> toSet() const { return Object::cast<>(const_cast<Set<T> *>(this)); }

    void toStringStream(std::ostream &os) override
    {
        os << '<' << typeid(T).name() << ">{ ";
        for (const auto &element : *this)
            os << element << ", ";
        os << "}";
    }

    template <typename R>
    ref<Set<R>> map(Function<R(const T &)>) const;
};
