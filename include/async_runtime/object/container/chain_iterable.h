#pragma once
#include "iterable.h"

template <typename T>
class ChainIterable : public Iterable<T>
{
protected:
    ref<List<ref<Iterable<T>>>> _iterables;
    option<ConstIterator<T>> _empty;

public:
    class EmptyConstIterator : public ConstIterator<T>
    {
        ref<ConstIterator<T>> next() const override { throw std::range_error("ConstIterator access overflow"); }
        const T &value() const override { throw std::range_error("ConstIterator access overflow"); }
    };

    class ChainIterableConstIterator : public ConstIterator<T>
    {
    public:
        ref<List<ref<Iterable<T>>>> iterables;
        const size_t current;
        ref<ConstIterator<T>> origin;

        ChainIterableConstIterator(ref<List<ref<Iterable<T>>>> iterables, size_t current, ref<ConstIterator<T>> origin)
            : iterables(iterables), current(current), origin(origin) {}

        ref<ConstIterator<T>> next() const override
        {
            auto n = origin.get()->next();
            ref<Iterable<T>> iterable = iterables[current];
            if (n == iterable.get()->end() && current != iterables->size() - 1)
            {
                size_t nextCurrent = current + 1;
                ref<ConstIterator<T>> begin = iterables[nextCurrent]->begin();
                return Object::create<ChainIterableConstIterator>(iterables, nextCurrent, begin);
            }
            return Object::create<ChainIterableConstIterator>(iterables, current, n);
        }

        const T &value() const override { return origin.get()->value(); }

        bool operator==(ref<Object> other) override
        {
            if (auto ptr = dynamic_cast<ChainIterableConstIterator *>(other.get()))
            {
                if (current == ptr->current)
                {
                    return origin == ptr->origin;
                }
            }
            return false;
        }
    };

    static ref<ChainIterable> from(ref<List<ref<Iterable<T>>>> iterables)
    {
        return Object::create<ChainIterable<int>>(iterables);
    }

    ChainIterable(ref<List<ref<Iterable<T>>>> iterables) : _iterables(iterables)
    {
        if (_iterables->isEmpty())
            _empty = Object::create<EmptyConstIterator>();
    }

    size_t size() const override
    {
        size_t sum(0);
        for (const ref<Iterable<T>> &iterable : _iterables)
            sum += iterable->size();
        return sum;
    }

    ref<ConstIterator<T>> begin() const override
    {
        return _empty.ifNotNullElse([&] { //
            return Object::create<ChainIterableConstIterator>(_iterables, 0, _iterables[0]->begin());
        });
    }

    ref<ConstIterator<T>> end() const override
    {
        return _empty.ifNotNullElse([&] { //
            size_t last = _iterables->size() - 1;
            return Object::create<ChainIterableConstIterator>(_iterables, last, _iterables[last]->end());
        });
    }
};
