#include "async_runtime/elements/stateless_element.h"
#include "async_runtime/widgets/stateless_widget.h"

ref<Element> StatelessWidget::createElement()
{
    return Object::create<StatelessElement>(self());
}