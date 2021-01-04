#pragma once

#include <utility>
#include <assert.h>
#include "value_listenable.h"

template <typename T>
class ValueNotifier : public virtual ValueListenable<T>, public virtual ChangeNotifier
{
public:
    ValueNotifier() = delete;
    ValueNotifier(T value) : _value(value) {}
    virtual void setValue(T &&value);
    virtual T &getValue() override;
    virtual const T &getValue() const override;

protected:
    T _value;
};

template <typename T>
void ValueNotifier<T>::setValue(T &&value)
{
    assert(!this->_isDisposed && "Can't access [setValue] after [dispose]");
    if (value != this->_value)
    {
        this->_value = value;
        this->notifyListeners();
    }
}

template <typename T>
T &ValueNotifier<T>::getValue()
{
    assert(!this->_isDisposed && "Can't access [getValue] after [dispose]");
    return this->_value;
}

template <typename T>
const T &ValueNotifier<T>::getValue() const
{
    assert(!this->_isDisposed && "Can't access [getValue] after [dispose]");
    return this->_value;
}