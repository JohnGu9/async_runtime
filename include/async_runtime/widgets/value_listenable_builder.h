#pragma once

#include "../basic/value_notifier.h"
#include "stateful_widget.h"

template <typename T = Object>
class ValueListenableBuilder : public StatefulWidget
{
public:
    ValueListenableBuilder(ref<ValueListenable<T>> valueListenable,
                           Function<ref<Widget>(
                               ref<BuildContext> context,
                               T value,
                               option<Widget> child)>
                               builder,
                           option<Widget> child = nullptr,
                           option<Key> key = nullptr)
        : StatefulWidget(key), builder(builder), child(child), valueListenable(valueListenable)
    {
    }

    Function<ref<Widget>(ref<BuildContext>, T, option<Widget>)> builder;
    finaloption<Widget> child;
    finalref<ValueListenable<T>> valueListenable;
    ref<State<StatefulWidget>> createState() override;

protected:
    class _State;
};

template <typename T>
class ValueListenableBuilder<T>::_State : public State<ValueListenableBuilder<T>>
{
public:
    using super = State<ValueListenableBuilder<T>>;
    lateref<ValueListenable<T>> _valueListenable;

    Function<void()> _listener = [this] { this->setState([this] {}); };

    void initState() override
    {
        super::initState();
        ref<ValueListenableBuilder<T>::_State> self = self();
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
