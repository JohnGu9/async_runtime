#pragma once
#include "iterable.h"

// @TODO: fix linux memory access bug
template <typename From, typename To>
class MapIterable : public Iterable<To>
{
    ref<Iterable<From>> _origin;
    Function<const To &(const From &)> _transform;

public:
    class MapIterableIterator : public ConstIterator<To>
    {
        ref<ConstIterator<From>> _other;
        Function<const To &(const From &)> _transform;

    public:
        MapIterableIterator(ref<ConstIterator<From>> other, Function<To(const From &)> transform)
            : _other(other), _transform(transform) {}

        ref<ConstIterator<From>> origin() const
        {
            return _other;
        }

        ref<ConstIterator<To>> next() const override
        {
            return Object::create<MapIterableIterator>(_other->next(), _transform);
        }

        const To &value() const override
        {
            return _transform(*_other);
        }

        bool operator==(ref<Object> other) override
        {
            if (auto ptr = dynamic_cast<MapIterableIterator *>(other.get())) // [[likely]]
            {
                return this->_other == ptr->_other;
            }
            return false;
        }
    };

    static ref<MapIterable<From, To>> from(ref<Iterable<From>> origin, Function<const To &(const From &)> transform);

    MapIterable(ref<Iterable<From>> origin, Function<const To &(const From &)> transform)
        : _origin(origin), _transform(transform) {}

    size_t size() const override { return _origin->size(); }
    ref<ConstIterator<To>> begin() const override { return Object::create<MapIterableIterator>(_origin->begin(), _transform); }
    ref<ConstIterator<To>> end() const override { return Object::create<MapIterableIterator>(_origin->end(), _transform); }

    void toStringStream(std::ostream &os) override
    {
        os << '<' << typeid(From).name() << " -> " << typeid(To).name() << ">[ ";
        for (const To &element : *this)
            os << element << ", ";
        os << "]";
    }
};

template <typename From, typename To>
ref<MapIterable<From, To>> from(ref<Iterable<From>> origin, Function<const To &(const From &)> transform)
{
    return Object::create(origin, transform);
}
