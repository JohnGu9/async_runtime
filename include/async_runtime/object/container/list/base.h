#pragma once

#include "../list.h"

template <typename T>
class List : public MutableIterable<T>,
             public IndexableMixin<size_t, T>
{
    _ASYNC_RUNTIME_FRIEND_FAMILY;

protected:
    List() {}

public:
    static ref<List<T>> create();

    virtual ref<List<T>> copy() const = 0;
    virtual ref<Set<T>> toSet() const;

    template <typename R>
    ref<List<R>> map(Function<R(const T &)>) const;

    virtual void removeAt(const size_t index) = 0;
    virtual bool insert(size_t index, const T &value) = 0;
    virtual bool insert(size_t index, T &&value) { return this->insert(index, static_cast<const T &>(value)); }

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
};
