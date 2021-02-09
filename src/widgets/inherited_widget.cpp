#include "async_runtime/elements/inherited_element.h"
#include "async_runtime/widgets/inherited_widget.h"

InheritedWidget::InheritedWidget(Object::Ref<Widget> child, Object::Ref<Key> key) : _child(child), StatelessWidget(key) { assert(_child != nullptr); }

Object::Ref<Widget> InheritedWidget::build(Object::Ref<BuildContext> context) { return _child; }

Object::Ref<Element> InheritedWidget::createElement()
{
    return Object::create<InheritedElement>(Object::cast<>(this));
}