#pragma once

#include "iterable.h"
#include "iterator.h"

template <typename T>
class ReverseAbleIterable;

template <typename T>
class ReverseIterable;

template <typename T>
class ReverseAbleIterable : public virtual Iterable<T>
{
public:
    virtual ref<ConstIterator<T>> rbegin() const = 0;
    virtual ref<ConstIterator<T>> rend() const = 0;
    virtual ref<ReverseAbleIterable<T>> toReserve() const;
};

template <typename T>
class ReverseAbleMuteIterable : public virtual ReverseAbleIterable<T>, public virtual MutableIterable<T>
{
public:
    virtual ref<Iterator<T>> rbegin() = 0;
    virtual ref<Iterator<T>> rend() = 0;
    virtual ref<ReverseAbleMuteIterable<T>> toReserve();
};

template <typename T>
class ReverseIterableWrapper : public virtual ReverseAbleIterable<T>
{
protected:
    ref<ReverseAbleIterable<T>> _origin;

public:
    ReverseIterableWrapper(ref<ReverseAbleIterable<T>> origin) : _origin(origin) {}
    size_t size() const override { return _origin->size(); }

    ref<ConstIterator<T>> begin() const override { return _origin->rbegin(); }
    ref<ConstIterator<T>> end() const override { return _origin->rend(); }
    ref<ConstIterator<T>> rbegin() const override { return _origin->begin(); }
    ref<ConstIterator<T>> rend() const override { return _origin->end(); }
    ref<ReverseAbleIterable<T>> toReserve() const override { return _origin; }
};

template <typename T>
class ReverseMuteIterableWrapper : public virtual ReverseAbleMuteIterable<T>
{
protected:
    ref<ReverseAbleMuteIterable<T>> _origin;

public:
    ReverseMuteIterableWrapper(ref<ReverseAbleMuteIterable<T>> origin) : _origin(origin) {}

    ref<ConstIterator<T>> begin() const override { return _origin->rbegin(); }
    ref<ConstIterator<T>> end() const override { return _origin->rend(); }
    ref<ConstIterator<T>> rbegin() const override { return _origin->begin(); }
    ref<ConstIterator<T>> rend() const override { return _origin->end(); }
    ref<ReverseAbleIterable<T>> toReserve() const override { return _origin; }

    ref<Iterator<T>> begin() override { return _origin->rbegin(); }
    ref<Iterator<T>> end() override { return _origin->rend(); }
    ref<Iterator<T>> rbegin() override { return _origin->begin(); }
    ref<Iterator<T>> rend() override { return _origin->end(); }
    ref<ReverseAbleMuteIterable<T>> toReserve() override { return _origin; }
};

template <typename T>
ref<ReverseAbleIterable<T>> ReverseAbleIterable<T>::toReserve() const
{
    return Object::create<ReverseIterableWrapper<T>>(Object::cast<>(const_cast<ReverseAbleIterable<T> *>(this)));
}

template <typename T>
ref<ReverseAbleMuteIterable<T>> ReverseAbleMuteIterable<T>::toReserve()
{
    return Object::create<ReverseMuteIterableWrapper<T>>(Object::cast<>(const_cast<ReverseAbleMuteIterable<T> *>(this)));
}