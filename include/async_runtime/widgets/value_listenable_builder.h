#pragma once
#include "../utilities/value_notifier.h"
#include "stateful_widget.h"

template <typename T = Object>
class ValueListenableBuilder : public StatefulWidget
{
public:
    ValueListenableBuilder(ref<ValueListenable<T>> valueListenable,
                           Function<ref<Widget>(ref<BuildContext> context, T value, option<Widget> child)> builder,
                           option<Widget> child = nullptr,
                           option<Key> key = nullptr)
        : StatefulWidget(key), valueListenable(valueListenable), builder(builder), child(child) {}
    finalref<ValueListenable<T>> valueListenable;
    finalref<Fn<ref<Widget>(ref<BuildContext>, T, option<Widget>)>> builder;
    finaloption<Widget> child;

protected:
    class _State;
    ref<State<StatefulWidget>> createState() override;
};

template <typename T>
class ValueListenableBuilder<T>::_State : public State<ValueListenableBuilder<T>>
{
    using super = State<ValueListenableBuilder<T>>;

public:
    lateref<ValueListenable<T>> _valueListenable;

    Function<void()> _listener = [this]
    { this->setState([] {}); };

    void initState() override
    {
        super::initState();
        this->_valueListenable = this->widget->valueListenable;
        this->_valueListenable->addListener(_listener);
    }

    void didWidgetUpdated(ref<ValueListenableBuilder<T>> oldWidget) override
    {
        super::didWidgetUpdated(oldWidget);
        ref<ValueListenable<T>> newListenable = this->widget->valueListenable;
        if (newListenable != _valueListenable)
        {
            this->_valueListenable->removeListener(this->_listener);
            this->_valueListenable = newListenable;
            this->_valueListenable->addListener(this->_listener);
        }
    }

    void dispose() override
    {
        this->_valueListenable->removeListener(this->_listener);
        super::dispose();
    }

    ref<Widget> build(ref<BuildContext> context) override
    {
        return this->widget->builder(
            context,
            this->_valueListenable->getValue(),
            this->widget->child);
    }
};

template <typename T>
inline ref<State<StatefulWidget>> ValueListenableBuilder<T>::createState()
{
    return Object::create<ValueListenableBuilder<T>::_State>();
}
