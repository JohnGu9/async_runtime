#pragma once
#include "../iterable.h"

template <typename T, typename IteratorType>
class IteratorWrapperIterable : public Iterable<T>
{
protected:
    const IteratorType _begin;
    const IteratorType _end;

public:
    class _ConstIterator : public ConstIterator<T>
    {
    public:
        const IteratorType _it;
        _ConstIterator(IteratorType it) : _it(it) {}

        ref<ConstIterator<T>> next() const override
        {
            auto copy = _it;
            return Object::create<_ConstIterator>(std::move(++copy));
        }

        const T &value() const override { return *_it; }

        bool operator==(ref<Object> other) override
        {
            if (auto ptr = dynamic_cast<_ConstIterator *>(other.get()))
            {
                return ptr->_it == _it;
            }
            return false;
        }
    };
    static ref<IteratorWrapperIterable<T, IteratorType>> from(const IteratorType begin, const IteratorType end)
    {
        return Object::create<IteratorWrapperIterable<T, IteratorType>>(std::move(begin), std::move(end));
    }

    IteratorWrapperIterable(const IteratorType begin, const IteratorType end)
        : _begin(std::move(begin)), _end(std::move(end)) {}

    size_t size() const
    {
        auto begin = _begin;
        size_t result = 0;
        while (begin != _end)
        {
            ++result;
        }
        return result;
    }

    ref<ConstIterator<T>> begin() const
    {
        return Object::create<_ConstIterator>(_begin);
    }

    ref<ConstIterator<T>> end() const
    {
        return Object::create<_ConstIterator>(_end);
    }
};
