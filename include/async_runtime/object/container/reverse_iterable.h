#pragma once

#include "iterable.h"
#include "iterator.h"

template <typename T>
class ReverseIterableMixin;

template <typename T>
class ReverseIterable : public Iterable<T>
{
    ref<Iterable<T>> _origin;
    ref<ReverseIterableMixin<T>> _mixin;

public:
    ReverseIterable(ref<ReverseIterableMixin<T>> mixin);
    size_t size() const override { return _origin->size(); }
    ref<ConstIterator<T>> begin() const override;
    ref<ConstIterator<T>> end() const override;
};

template <typename T>
class ReverseIterableMixin : public virtual Object
{
public:
    virtual ref<ConstIterator<T>> rbegin() const = 0;
    virtual ref<ConstIterator<T>> rend() const = 0;
    virtual ref<ReverseIterable<T>> toReserve() const { return Object::create<ReverseIterable<T>>(Object::cast<>(const_cast<ReverseIterableMixin *>(this))); }
};

template <typename T>
ReverseIterable<T>::ReverseIterable(ref<ReverseIterableMixin<T>> mixin)
    : _origin(mixin->template covariant<Iterable<T>>()), _mixin(mixin) {}

template <typename T>
ref<ConstIterator<T>> ReverseIterable<T>::begin() const { return _mixin->rbegin(); }

template <typename T>
ref<ConstIterator<T>> ReverseIterable<T>::end() const { return _mixin->rend(); }