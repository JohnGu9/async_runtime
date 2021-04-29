#pragma once

#include "../object.h"
#include "lock.h"

template <typename T>
class Iterable : public Object
{
public:
    Iterable() : _lock(Object::create<Lock>()) {}
    virtual bool any(Function<bool(const T &)> fn) const = 0;
    virtual bool every(Function<bool(const T &)> fn) const = 0;

protected:
    ref<Lock> _lock;

public:
    const ref<Lock> &lock = _lock;
};

#include "container/list.h"
#include "container/set.h"
#include "container/map.h"
