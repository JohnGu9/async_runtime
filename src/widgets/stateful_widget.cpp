#include "async_runtime/widgets/stateful_widget.h"

StatefulWidget::StatefulWidget(Object::Ref<Key> key) : Widget(key) {}

Object::Ref<Element> StatefulWidget::createElement()
{
    return Object::create<StatefulElement>(Object::cast<>(this));
}
