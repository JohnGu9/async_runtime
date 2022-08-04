#pragma once
#include "../iterable.h"
#include <iterator>

namespace _async_runtime
{
    template <typename T>
    struct RemoveConstReference
    {
        using type = typename std::remove_const<typename std::remove_reference<T>::type>::type;
    };
};

template <typename T = Object::Void, typename IteratorType = Object::Void>
class IteratorWrapperIterable;

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

        bool operator==(const option<Object> &other) override
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
        return std::distance(_begin, _end);
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

template <>
class IteratorWrapperIterable<Object::Void, Object::Void> : public Object
{
public:
    template <typename IteratorType>
    static auto from(const IteratorType begin, const IteratorType end)
        -> ref<IteratorWrapperIterable<typename _async_runtime::RemoveConstReference<decltype(*begin)>::type, IteratorType>>
    {
        return Object::create<IteratorWrapperIterable<typename _async_runtime::RemoveConstReference<decltype(*begin)>::type, IteratorType>>(std::move(begin), std::move(end));
    }

    template <typename Container>
    static auto from(Container &container)
        -> decltype(IteratorWrapperIterable<Object::Void, Object::Void>::from(container.begin(), container.end()))
    {
        return IteratorWrapperIterable<Object::Void, Object::Void>::from(container.begin(), container.end());
    }
};
