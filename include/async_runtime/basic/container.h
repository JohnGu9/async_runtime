#pragma once

#include "function.h"
#include "object.h"

template <typename T>
class Iterable : public virtual Object
{
public:
    virtual bool any(Function<bool(const T &)> fn) const = 0;
    virtual bool every(Function<bool(const T &)> fn) const = 0;
    virtual void forEach(Function<void(const T &)>) const = 0;
};

template <typename T>
class Removable
{
public:
    virtual bool remove(T) = 0;
};

template <typename T>
class Containable
{
public:
    virtual bool contain(T) = 0;
};

#include "container/list.h"
#include "container/map.h"
#include "container/set.h"
