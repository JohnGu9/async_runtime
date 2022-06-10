#pragma once

#include "../list.h"

template <typename T>
class List : public MutableIterable<T>,
             public IndexableMixin<size_t, T>,
             public InsertMixin<T>
{
    _ASYNC_RUNTIME_FRIEND_FAMILY;

protected:
    List() {}

public:
    static ref<List<T>> create();

    virtual ref<List<T>> copy() const = 0;

    template <typename R>
    ref<List<R>> map(Function<R(const T &)>) const;

    virtual void removeAt(const size_t index) = 0;

    virtual void emplaceBack(const T &value) { this->emplace(value); }
    virtual void emplaceBack(T &&value) { this->emplace(std::move(value)); }

    virtual void popBack()
    {
        auto index = this->size();
        if (index > 0)
            this->removeAt(index - 1);
    }

    void toStringStream(std::ostream &os) override
    {
        os << '<' << typeid(T).name() << ">[ ";
        for (const auto &element : *this)
            os << element << ", ";
        os << "]";
    }
};
