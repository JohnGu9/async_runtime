#include "async_runtime/elements/inherited_element.h"
#include "async_runtime/widgets/inherited_widget.h"

InheritedWidget::InheritedWidget(ref<Widget> child, option<Key> key) : _child(child), StatelessWidget(key) {}

ref<Widget> InheritedWidget::build(ref<BuildContext> context) { return _child; }

ref<Element> InheritedWidget::createElement()
{
    return Object::create<InheritedElement>(Object::cast<>(this));
}