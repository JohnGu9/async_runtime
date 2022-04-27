#include "async_runtime/widgets/root_widget.h"
#include "async_runtime/elements/root_element.h"

ref<RootWidget> RootWidget::of(ref<BuildContext> context)
{
    return context->dependOnInheritedWidgetOfExactType<RootWidget>().assertNotNull();
}

RootWidget::RootWidget(ref<Widget> child, RootElement *element, option<Key> key)
    : InheritedWidget(child, key), exit(element->exit), cout(element->cout), element(element) {}
