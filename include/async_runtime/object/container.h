#pragma once

#include "function.h"
#include "object.h"

template <typename T>
class ContainableMixin
{
public:
    virtual bool contain(const T &other) const = 0;
    virtual bool contain(T &&other) const { return this->contain(static_cast<const T &>(other)); }
};

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
class RemovableMixin
{
public:
    virtual bool remove(const T &) = 0;
    virtual bool remove(T &&v) { return this->remove(static_cast<const T &>(v)); }
};

#include "container/list.h"
#include "container/map.h"
#include "container/set.h"
