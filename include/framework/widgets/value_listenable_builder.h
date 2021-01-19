#pragma once

#include "widget.h"
#include "../basic/value_notifier.h"

template <typename T = Object>
class ValueListenableBuilder : public StatefulWidget
{
public:
    ValueListenableBuilder(Fn<Object::Ref<Widget>(Object::Ref<BuildContext>, T, Object::Ref<Widget>)> builder,
                           Object::Ref<ValueListenable<T>> valueListenable,
                           Object::Ref<Widget> child = nullptr,
                           Object::Ref<Key> key = nullptr)
        : builder(builder), child(child), valueListenable(valueListenable), StatefulWidget(key) {}
    Fn<Object::Ref<Widget>(Object::Ref<BuildContext>, T, Object::Ref<Widget>)> builder;
    Object::Ref<Widget> child;
    Object::Ref<ValueListenable<T>> valueListenable;
    Object::Ref<State> createState() override;
};

template <typename T>
class _ValueListenableBuilderState : public State<ValueListenableBuilder<T>>
{
    using super = State<ValueListenableBuilder<T>>;
    T _value;
    Object::Ref<Fn<void(Object::Ref<Listenable>)>> _listener;
    Object::Ref<ValueListenable<T>> _valueListenable;

    void initState() override
    {
        super::initState();
        Object::Ref<_ValueListenableBuilderState> self = Object::self(this);
        _listener = Object::create<Fn<void(Object::Ref<Listenable>)>>(
            [self](Object::Ref<Listenable> listenable) {
                Object::Ref<ValueListenable<T>> valueListenable = listenable->cast<ValueListenable<T>>();
                if (self->getMounted())
                    self->setState([self, valueListenable] { self->_value = valueListenable->getValue(); });
            });
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
inline Object::Ref<StatefulWidget::State> ValueListenableBuilder<T>::createState()
{
    return Object::create<_ValueListenableBuilderState<T>>();
}
