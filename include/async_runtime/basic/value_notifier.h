#pragma once

#include <utility>
#include <assert.h>
#include "value_listenable.h"

template <typename T>
class ValueNotifier : public virtual ValueListenable<T>, public virtual ChangeNotifier
{
public:
    static Object::Ref<ValueNotifier<T>> fromValue(T value) { return Object::create<ValueNotifier<T>>(value); }
    ValueNotifier() = delete;
    ValueNotifier(T value) : _value(value) {}
    virtual void setValue(const T &value);
    virtual void setValue(T &&value);

    T &getValue() override;
    const T &getValue() const override;

protected:
    T _value;
};

template <typename T>
void ValueNotifier<T>::setValue(const T &value)
{
    assert(!this->_isDisposed && "Can't access [setValue] after [dispose]");
    if (value != this->_value)
    {
        this->_value = value;
        this->notifyListeners();
    }
}

template <typename T>
void ValueNotifier<T>::setValue(T &&value)
{
    assert(!this->_isDisposed && "Can't access [setValue] after [dispose]");
    if (value != this->_value)
    {
        this->_value = std::forward<T>(value);
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