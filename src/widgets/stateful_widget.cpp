#include "async_runtime/widgets/stateful_widget.h"

Object::Ref<Element> StatefulWidget::createElement()
{
    return Object::create<StatefulElement>(Object::cast<>(this));
}
