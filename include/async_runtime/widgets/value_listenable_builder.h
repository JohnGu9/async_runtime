#pragma once

#include "widget.h"
#include "../widgets/state.h"
#include "../basic/value_notifier.h"

template <typename T = Object>
class ValueListenableBuilder : public StatefulWidget
{
public:
    ValueListenableBuilder(Object::Ref<ValueListenable<T>> valueListenable,
                           Function<Object::Ref<Widget>(
                               Object::Ref<BuildContext> context,
                               T value,
                               Object::Ref<Widget> child)>
                               builder,
                           Object::Ref<Widget> child = nullptr,
                           Object::Ref<Key> key = nullptr)
        : builder(builder), child(child), valueListenable(valueListenable), StatefulWidget(key)
    {
    }

    Function<Object::Ref<Widget>(Object::Ref<BuildContext>, T, Object::Ref<Widget>)> builder;
    Object::Ref<Widget> child;
    Object::Ref<ValueListenable<T>> valueListenable;
    Object::Ref<State<StatefulWidget>> createState() override;

protected:
    class _State;
};

template <typename T>
class ValueListenableBuilder<T>::_State : public State<ValueListenableBuilder<T>>
{
public:
    using super = State<ValueListenableBuilder<T>>;
    Object::Ref<ValueListenable<T>> _valueListenable;

    Function<void(Object::Ref<Listenable>)> _listener =
        [this](Object::Ref<Listenable> listenable) { this->setState([this] {}); };

    void initState() override
    {
        super::initState();
        Object::Ref<ValueListenableBuilder<T>::_State> self = Object::cast<>(this);
        this->_valueListenable = this->getWidget()->valueListenable;
        this->_valueListenable->addListener(_listener);
    }

    void didWidgetUpdated(Object::Ref<StatefulWidget> oldWidget) override
    {
        Object::Ref<ValueListenable<T>> newListenable = this->getWidget()->valueListenable;
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

    Object::Ref<Widget> build(Object::Ref<BuildContext> context) override
    {
        return this->getWidget()->builder(
            context,
            this->_valueListenable->getValue(),
            this->getWidget()->child);
    }
};

template <typename T>
inline Object::Ref<State<StatefulWidget>> ValueListenableBuilder<T>::createState()
{
    return Object::create<ValueListenableBuilder<T>::_State>();
}
