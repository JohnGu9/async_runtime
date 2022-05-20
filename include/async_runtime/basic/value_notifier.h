#pragma once

#include "value_listenable.h"
#include <assert.h>
#include <utility>

template <typename T = std::nullptr_t>
class ValueNotifier;

template <>
class ValueNotifier<std::nullptr_t> : public ChangeNotifier
{
protected:
    ValueNotifier(){};
};

template <typename T>
class ValueNotifier : public ValueListenable<T>, public ValueNotifier<std::nullptr_t>, public setter<T>
{
public:
    static ref<ValueNotifier<T>> fromValue(const T &value) { return Object::create<ValueNotifier<T>>(std::forward<T>(value)); }
    static ref<ValueNotifier<T>> fromValue(T &&value) { return Object::create<ValueNotifier<T>>(std::forward<T>(value)); }
    ValueNotifier() = delete;
    ValueNotifier(const T &value) : _value(value) {}
    ValueNotifier(T &&value) : _value(value) {}

    void setValue(const T &value) override;
    void setValue(T &&value) override;
    T &getValue() override;
    const T &getValue() const override;

protected:
    T _value;

public:
    const T &value = _value;
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
