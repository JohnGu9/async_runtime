#pragma once
#include "map_iterable.h"

/**
 * @brief Similar to MapIterable. But this receive a transform that return a RIGHT value
 * Not to overuse this that may cause performance issue.
 *
 * @tparam From
 * @tparam To
 */
template <typename From, typename To>
class CachedMapIterable : public MapIterable<From, To>
{
    using super = MapIterable<From, To>;

protected:
    Function<To(const From &)> _cachedTransform;

public:
    class _EndConstIterator : public ConstIterator<To>
    {
    public:
        ref<ConstIterator<From>> _other;
        _EndConstIterator(ref<ConstIterator<From>> other)
            : _other(other) {}

        ref<ConstIterator<To>> next() const override
        {
            throw NotImplementedError();
        }

        const To &value() const override
        {
            throw NotImplementedError();
        }

        bool operator==(ref<Object> other) override
        {
            if (auto ptr = dynamic_cast<_EndConstIterator *>(other.get())) // [[likely]]
            {
                return this->_other == ptr->_other;
            }
            return false;
        }
    };

    class _ConstIterator : public _EndConstIterator
    {
        using super = _EndConstIterator;
        ref<ConstIterator<From>> _end;
        Function<To(const From &)> _transform;
        To _value;

    public:
        _ConstIterator(ref<ConstIterator<From>> other, ref<ConstIterator<From>> end, Function<To(const From &)> transform)
            : super(other), _end(end), _transform(transform), _value(transform(*other)) {}

        ref<ConstIterator<To>> next() const override
        {
            auto next = this->_other.get()->next();
            if (next == _end)
            {
                return Object::create<_EndConstIterator>(next);
            }
            return Object::create<_ConstIterator>(next, _end, _transform);
        }

        const To &value() const override
        {
            return _value;
        }
    };

    static ref<CachedMapIterable<From, To>> from(ref<Iterable<From>> origin, Function<To(const From &)> transform);

    CachedMapIterable(ref<Iterable<From>> origin, Function<To(const From &)> transform)
        : super(origin, [](const From &) -> const To &
                { throw NotImplementedError(); }),
          _cachedTransform(transform)
    {
    }

    ref<ConstIterator<To>> begin() const override { return Object::create<_ConstIterator>(this->_origin->begin(), this->_origin->end(), _cachedTransform); }
    ref<ConstIterator<To>> end() const override { return Object::create<_EndConstIterator>(this->_origin->end()); }
};

template <typename From, typename To>
ref<CachedMapIterable<From, To>> CachedMapIterable<From, To>::from(ref<Iterable<From>> origin, Function<To(const From &)> transform)
{
    return Object::create<CachedMapIterable<From, To>>(origin, transform);
}
