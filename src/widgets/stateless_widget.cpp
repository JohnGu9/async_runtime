#include "async_runtime/elements/stateless_element.h"
#include "async_runtime/widgets/stateless_widget.h"

Object::Ref<Element> StatelessWidget::createElement()
{
    return Object::create<StatelessElement>(Object::cast<>(this));
}