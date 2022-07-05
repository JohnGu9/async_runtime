#pragma once
#include "../iterable.h"

class RangeIterable : public Iterable<int>
{
protected:
    int _begin;
    int _end;
    int _step;

public:
    class _End : public ConstIterator<int>
    {
        ref<ConstIterator<int>> next() const override { throw NotImplementedError(); }
        const int &value() const override { throw NotImplementedError(); }
        bool operator==(option<Object> other) override
        {
            if (auto ptr = dynamic_cast<_End *>(other.get()))
            {
                return true;
            }
            return false;
        }
    };
    class _ConstIterator : public ConstIterator<int>
    {
    public:
        int _current;
        int _end;
        int _step;

        _ConstIterator(int current, int end, int step) : _current(current), _end(end), _step(step) {}
        ref<ConstIterator<int>> next() const override
        {
            auto nextValue = _current + _step;
            if (nextValue >= _end)
                return Object::create<_End>();
            return Object::create<_ConstIterator>(nextValue, _end, _step);
        }
        const int &value() const override { return _current; }
        bool operator==(option<Object> other) override
        {
            if (auto ptr = dynamic_cast<_ConstIterator *>(other.get()))
            {
                return this->_current == ptr->_current;
            }
            return false;
        }
    };

protected:
    ref<ConstIterator<int>> _endIterator = Object::create<_End>();

public:
    RangeIterable(int begin, int end) : _begin(begin), _end(end), _step(begin < end ? 1 : -1) {}
    RangeIterable(int begin, int end, int step) : _begin(begin), _end(end), _step(step) {}

    size_t size() const override { return (_end - _begin) / _step; }
    ref<ConstIterator<int>> begin() const override
    {
        if (_begin == _end)
            return _endIterator;
        return Object::create<_ConstIterator>(_begin, _end, _step);
    }
    ref<ConstIterator<int>> end() const override { return _endIterator; }

    static ref<RangeIterable> from(int begin, int end)
    {
        return Object::create<RangeIterable>(begin, end);
    }
    static ref<RangeIterable> from(int begin, int end, int step)
    {
        return Object::create<RangeIterable>(begin, end, step);
    }
};
