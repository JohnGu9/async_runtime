#pragma once

#include "../list.h"

template <typename T>
class Set : public MutableIterable<T>
{
    _ASYNC_RUNTIME_FRIEND_FAMILY;

protected:
    Set() {}

public:
    static ref<Set<T>> create();

    virtual ref<Set<T>> copy() const = 0;
    virtual ref<List<T>> toList() const;

    template <typename R>
    ref<Set<R>> map(Function<R(const T &)>) const;

    virtual ref<Iterator<T>> find(const T &) = 0;
    virtual ref<Iterator<T>> find(T &&key) { return this->find(static_cast<const T &>(key)); }

    virtual ref<ConstIterator<T>> find(const T &) const = 0;
    virtual ref<ConstIterator<T>> find(T &&key) const { return this->find(static_cast<const T &>(key)); }

    bool contains(const T &other) const override { return this->find(other) != this->end(); }

    void toStringStream(std::ostream &os) override
    {
        os << '<' << typeid(T).name() << ">{ ";
        for (const auto &element : *this)
            os << element << ", ";
        os << "}";
    }
};
