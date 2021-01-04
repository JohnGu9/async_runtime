#include "framework/widgets/widget.h"

Widget::Widget(Object::Ref<Key> key) : _key(key) {}

bool Widget::canUpdate(Object::Ref<Widget> other)
{
    if (other == nullptr)
        return false;
    return this->runtimeType() == other->runtimeType() && this->getKey() == other->getKey();
}

LeafWidget::LeafWidget(Object::Ref<Key> key) : Widget(key){};

Object::Ref<Element> LeafWidget::createElement()
{
    return Object::create<LeafElement>(Object::cast<LeafWidget>(this));
}

StatelessWidget::StatelessWidget(Object::Ref<Key> key) : Widget(key) {}

Object::Ref<Element> StatelessWidget::createElement()
{
    return Object::create<StatelessElement>(Object::cast<StatelessWidget>(this));
}

StatefulWidget::StatefulWidget(Object::Ref<Key> key) : Widget(key) {}

Object::Ref<Element> StatefulWidget::createElement()
{
    return Object::create<StatefulElement>(Object::cast<StatefulWidget>(this));
}

StatefulWidgetState::StatefulWidgetState() : mounted(false) {}

void StatefulWidgetState::initState() {}

void StatefulWidgetState::didWidgetUpdated(Object::Ref<StatefulWidget> oldWidget) {}

void StatefulWidgetState::didDependenceChanged() {}

void StatefulWidgetState::dispose() {}

void StatefulWidgetState::setState(Object::Ref<Fn<void()>> fn)
{
    (*fn)();
    this->getElement()->build();
}

Object::Ref<BuildContext> StatefulWidgetState::getContext() { return this->getElement(); }

Object::Ref<StatefulElement> StatefulWidgetState::getElement()
{
    assert(this->mounted && "User can't access context before initState. You can try [didDependenceChanged] method to access context before first build. ");
    return this->element.lock();
}

InheritedWidget::InheritedWidget(Object::Ref<Widget> child, Object::Ref<Key> key) : _child(child), StatelessWidget(key) { assert(_child != nullptr); }

Object::Ref<Widget> InheritedWidget::build(Object::Ref<BuildContext> context) { return _child; }

Object::Ref<Element> InheritedWidget::createElement()
{
    return Object::create<InheritedElement>(Object::cast<InheritedWidget>(this));
}

NotificationListener::NotificationListener(Object::Ref<Widget> child, Object::Ref<Key> key) : _child(child), StatelessWidget(key) {}

Object::Ref<Widget> NotificationListener::build(Object::Ref<BuildContext> context)
{
    return this->_child;
}

Object::Ref<Element> NotificationListener::createElement()
{
    return Object::create<NotificationListenerElement>(Object::cast<NotificationListener>(this));
}

MultiChildWidget::MultiChildWidget(Object::List<Object::Ref<Widget>> children, Object::Ref<Key> key) : _children(children), Widget(key) {}

Object::Ref<Element> MultiChildWidget::createElement()
{
    return Object::create<MultiChildElement>(Object::cast<MultiChildWidget>(this));
}