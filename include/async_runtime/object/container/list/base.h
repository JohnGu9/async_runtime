#pragma once

#include "../list.h"

template <typename T>
class List : public MutableIterable<T>,
             public RemovableMixin<size_t>,
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

    void emplaceBack(const T &value) { this->emplace(value); }
    void emplaceBack(T &&value) { this->emplace(std::move(value)); }
    void popBack()
    {
        auto index = this->size();
        if (index > 0)
            static_cast<RemovableMixin<size_t> *>(this)->remove(index - 1);
    }

    void toStringStream(std::ostream &os) override
    {
        os << '<' << typeid(T).name() << ">[ ";
        for (const auto &element : *this)
            os << element << ", ";
        os << "]";
    }
};
