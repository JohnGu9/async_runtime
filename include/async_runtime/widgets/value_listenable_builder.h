#pragma once

#include "widget.h"
#include "../basic/value_notifier.h"

template <typename T = Object>
class ValueListenableBuilder : public StatefulWidget
{
public:
    ValueListenableBuilder(Function<Object::Ref<Widget>(Object::Ref<BuildContext> context, T value, Object::Ref<Widget> child)> builder,
                           Object::Ref<ValueListenable<T>> valueListenable,
                           Object::Ref<Widget> child = nullptr,
                           Object::Ref<Key> key = nullptr)
        : builder(builder), child(child), valueListenable(valueListenable), StatefulWidget(key) {}
    Function<Object::Ref<Widget>(Object::Ref<BuildContext>, T, Object::Ref<Widget>)> builder;
    Object::Ref<Widget> child;
    Object::Ref<ValueListenable<T>> valueListenable;
    Object::Ref<State<StatefulWidget>> createState() override;
};

template <typename T>
class _ValueListenableBuilderState : public State<ValueListenableBuilder<T>>
{
    using super = State<ValueListenableBuilder<T>>;
    T _value;
    Object::Ref<ValueListenable<T>> _valueListenable;

    Function<void(Object::Ref<Listenable>)> _listener = [this](Object::Ref<Listenable> listenable) {
        Object::Ref<ValueListenable<T>> valueListenable = listenable->cast<ValueListenable<T>>();
        if (this->getMounted())
            this->setState([this, valueListenable] { this->_value = valueListenable->getValue(); });
    };

    void initState() override
    {
        super::initState();
        Object::Ref<_ValueListenableBuilderState> self = Object::cast<>(this);
        _valueListenable = this->getWidget()->valueListenable;
        _valueListenable->addListener(_listener);
        _value = _valueListenable->getValue();
    }

    void didWidgetUpdated(Object::Ref<StatefulWidget> oldWidget) override
    {
        Object::Ref<ValueListenable<T>> newListenable = this->getWidget()->valueListenable;
        if (newListenable != _valueListenable)
        {
            _valueListenable->removeListener(_listener);
            _valueListenable = newListenable;
            _valueListenable->addListener(_listener);
            _value = _valueListenable->getValue();
        }
        super::didWidgetUpdated(oldWidget);
    }

    void dispose() override
    {
        _valueListenable->removeListener(_listener);
        super::dispose();
    }

    Object::Ref<Widget> build(Object::Ref<BuildContext> context) override
    {
        return this->getWidget()->builder(context, this->_value, this->getWidget()->child);
    }
};

template <typename T>
inline Object::Ref<State<StatefulWidget>> ValueListenableBuilder<T>::createState()
{
    return Object::create<_ValueListenableBuilderState<T>>();
}
