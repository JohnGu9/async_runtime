#pragma once

#include "../iterable.h"

template <typename T>
class ReversibleIterable : public virtual Iterable<T>
{
public:
    virtual ref<ConstIterator<T>> rbegin() const = 0;
    virtual ref<ConstIterator<T>> rend() const = 0;
    virtual ref<ReversibleIterable<T>> toReserve() const;
};

template <typename T>
class ReversibleMuteIterable : public virtual ReversibleIterable<T>, public virtual MutableIterable<T>
{
public:
    virtual ref<Iterator<T>> rbegin() = 0;
    virtual ref<Iterator<T>> rend() = 0;
    virtual ref<ReversibleMuteIterable<T>> toReserve();

    ref<ConstIterator<T>> rbegin() const override
    {
        return const_cast<ReversibleMuteIterable<T> *>(this)->rbegin().get()->toConst();
    }
    ref<ConstIterator<T>> rend() const override
    {
        return const_cast<ReversibleMuteIterable<T> *>(this)->rend().get()->toConst();
    }
};

template <typename T>
class ReverseIterableWrapper : public virtual ReversibleIterable<T>
{
protected:
    ref<ReversibleIterable<T>> _origin;

public:
    ReverseIterableWrapper(ref<ReversibleIterable<T>> origin) : _origin(origin) {}
    size_t size() const override { return _origin->size(); }

    ref<ConstIterator<T>> begin() const override { return _origin->rbegin(); }
    ref<ConstIterator<T>> end() const override { return _origin->rend(); }
    ref<ConstIterator<T>> rbegin() const override { return _origin->begin(); }
    ref<ConstIterator<T>> rend() const override { return _origin->end(); }
    ref<ReversibleIterable<T>> toReserve() const override { return _origin; }
};

template <typename T>
class ReverseMuteIterableWrapper : public virtual ReversibleMuteIterable<T>
{
protected:
    ref<ReversibleMuteIterable<T>> _origin;

public:
    ReverseMuteIterableWrapper(ref<ReversibleMuteIterable<T>> origin) : _origin(origin) {}

    ref<ConstIterator<T>> begin() const override { return _origin->rbegin(); }
    ref<ConstIterator<T>> end() const override { return _origin->rend(); }
    ref<ConstIterator<T>> rbegin() const override { return _origin->begin(); }
    ref<ConstIterator<T>> rend() const override { return _origin->end(); }
    ref<ReversibleIterable<T>> toReserve() const override { return _origin; }

    ref<Iterator<T>> begin() override { return _origin->rbegin(); }
    ref<Iterator<T>> end() override { return _origin->rend(); }
    ref<Iterator<T>> rbegin() override { return _origin->begin(); }
    ref<Iterator<T>> rend() override { return _origin->end(); }
    ref<ReversibleMuteIterable<T>> toReserve() override { return _origin; }
};

template <typename T>
ref<ReversibleIterable<T>> ReversibleIterable<T>::toReserve() const
{
    return Object::create<ReverseIterableWrapper<T>>(constSelf());
}

template <typename T>
ref<ReversibleMuteIterable<T>> ReversibleMuteIterable<T>::toReserve()
{
    return Object::create<ReverseMuteIterableWrapper<T>>(self());
}
