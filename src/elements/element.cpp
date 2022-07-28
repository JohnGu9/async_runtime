#include "async_runtime/elements/element.h"
#include "async_runtime/widgets/key.h"
#include "async_runtime/widgets/widget.h"

Element::Element(ref<Widget> widget) : BuildContext(widget) {}

void Element::initKey(ref<Widget> widget)
{
    widget->key.ifNotNull([&](ref<Key> key) { //
        key->setElement(self());
    });
}

void Element::disposeKey(ref<Widget> widget)
{
    static Function<void(ref<Key>)> fn = [](ref<Key> key) { // cache fn
        key->dispose();
    };
    widget->key.ifNotNull(fn);
}

void Element::attach()
{
    ref<Element> parent = this->parent.assertNotNull();
    this->_inheritances = parent->_inheritances;
    initKey(this->widget);
}

static finalref<Map<Object::RuntimeType, lateref<InheritedWidget>>> _empty = Map<Object::RuntimeType, lateref<InheritedWidget>>::create();
void Element::detach()
{
    disposeKey(this->widget);
    this->_inheritances = _empty; // release map reference
}

void Element::update(ref<Widget> newWidget)
{
    if (this->widget->key != newWidget->key)
    {
        disposeKey(this->widget);
        initKey(newWidget);
    }
    this->widget = newWidget;
    this->build();
}

void Element::notify(ref<Widget> newWidget)
{
    if (this->widget->key != newWidget->key)
    {
        disposeKey(this->widget);
        initKey(newWidget);
    }
    this->widget = newWidget;
    ref<Element> parent = this->parent.assertNotNull();
    this->_inheritances = parent->_inheritances; // update inheritances
}

void Element::visitAncestor(Function<bool(ref<Element>)> fn)
{
    ref<Element> parent = this->parent.assertNotNull();
    if (fn(parent) == false)
        parent->visitAncestor(fn);
}
