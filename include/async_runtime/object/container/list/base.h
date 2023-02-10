#pragma once
#include "../list.h"

template <typename T>
class List : public MutableIterable<T>,
             public IndexableMixin<size_t, T>
{
    _ASYNC_RUNTIME_FRIEND_FAMILY;

protected:
    List() noexcept = default;

public:
    static ref<List<T>> create();
    virtual ref<List<T>> copy() const = 0;

    virtual void removeAt(size_t index) = 0;

    virtual bool insertAt(size_t index, const T &value) = 0;
    virtual bool insertAt(size_t index, T &&value) = 0;

    virtual void emplaceBack(const T &value) { this->emplace(value); }
    virtual void emplaceBack(T &&value) { this->emplace(std::move(value)); }

    virtual void popFront()
    {
        if (this->size() != 0)
            this->removeAt(0);
    }

    virtual void popBack()
    {
        auto index = this->size();
        if (index > 0)
            this->removeAt(index - 1);
    }

    // ref<List<T>> toList() const override { return constSelf(); }

    void toStringStream(std::ostream &os) override
    {
        os << '<' << typeid(T).name() << ">[ ";
        for (const T &element : *this)
            os << element << ", ";
        os << "]";
    }

    template <typename R>
    ref<List<R>> map(Function<R(const T &)>) const;
};
