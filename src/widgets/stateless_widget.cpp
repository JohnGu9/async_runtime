#include "async_runtime/widgets/stateless_widget.h"

StatelessWidget::StatelessWidget(Object::Ref<Key> key) : Widget(key) {}

Object::Ref<Element> StatelessWidget::createElement()
{
    return Object::create<StatelessElement>(Object::cast<>(this));
}