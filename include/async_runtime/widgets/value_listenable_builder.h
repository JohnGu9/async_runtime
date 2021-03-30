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
        : builder(builder), child(child), valueListenable(valueListenable), StatefulWidget(key)
    {
    }

    Function<ref<Widget>(ref<BuildContext>, T, option<Widget>)> builder;
    option<Widget> child;
    ref<ValueListenable<T>> valueListenable;
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

    Function<void(ref<Listenable>)> _listener =
        [this](option<Listenable> listenable) { this->setState([this] {}); };

    void initState() override
    {
        super::initState();
        ref<ValueListenableBuilder<T>::_State> self = self();
        this->_valueListenable = this->getWidget()->valueListenable;
        this->_valueListenable->addListener(_listener);
    }

    void didWidgetUpdated(ref<StatefulWidget> oldWidget) override
    {
        ref<ValueListenable<T>> newListenable = this->getWidget()->valueListenable;
        if (newListenable != _valueListenable)
        {
            this->_valueListenable->removeListener(this->_listener);
            this->_valueListenable = newListenable;
            this->_valueListenable->addListener(this->_listener);
        }
        super::didWidgetUpdated(oldWidget);
    }

    void dispose() override
    {
        this->_valueListenable->removeListener(this->_listener);
        super::dispose();
    }

    ref<Widget> build(ref<BuildContext> context) override
    {
        return this->getWidget()->builder(
            context,
            this->_valueListenable->getValue(),
            this->getWidget()->child);
    }
};

template <typename T>
inline ref<State<StatefulWidget>> ValueListenableBuilder<T>::createState()
{
    return Object::create<ValueListenableBuilder<T>::_State>();
}
