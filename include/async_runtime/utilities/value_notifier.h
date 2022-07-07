#pragma once

#include "value_listenable.h"

template <typename T = Object::Void>
class ValueNotifier;

template <>
class ValueNotifier<Object::Void> : public ChangeNotifier
{
protected:
    ValueNotifier(){};
};

template <typename T>
class ValueNotifier : public ValueListenable<T>, public ValueNotifier<Object::Void>, public setter<T>
{
public:
    static ref<ValueNotifier<T>> fromValue(const T &value) { return Object::create<ValueNotifier<T>>(std::forward<T>(value)); }
    static ref<ValueNotifier<T>> fromValue(T &&value) { return Object::create<ValueNotifier<T>>(std::forward<T>(value)); }
    ValueNotifier() = delete;
    ValueNotifier(const T &value) : _value(value) {}
    ValueNotifier(T &&value) : _value(value) {}

    void setValue(const T &value) override;
    void setValue(T &&value) override;
    const T &getValue() const override;

protected:
    T _value;

public:
    const T &value = _value;
};

template <typename T>
void ValueNotifier<T>::setValue(const T &value)
{
    RUNTIME_ASSERT(!this->_isDisposed, "ValueNotifier can't access [setValue] after [dispose]");
    if (value != this->_value)
    {
        this->_value = value;
        this->notifyListeners();
    }
}

template <typename T>
void ValueNotifier<T>::setValue(T &&value)
{
    RUNTIME_ASSERT(!this->_isDisposed, "ValueNotifier can't access [setValue] after [dispose]");
    if (value != this->_value)
    {
        this->_value = std::forward<T>(value);
        this->notifyListeners();
    }
}

template <typename T>
const T &ValueNotifier<T>::getValue() const
{
    RUNTIME_ASSERT(!this->_isDisposed, "ValueNotifier can't access [getValue] after [dispose]");
    return this->_value;
}
