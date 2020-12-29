#include "framework/widgets/widget.h"

Widget::Widget(Object::Ref<Key> key) : _key(key) {}

bool Widget::canUpdate(Object::Ref<Widget> other)
{
    if (other == nullptr)
        return false;
    return this->runtimeType() == other->runtimeType() && this->getKey() == other->getKey();
}

LeafWidget::LeafWidget(Object::Ref<Key> key) : Widget(key){};

InheritedWidget::InheritedWidget(Object::Ref<Widget> child) : _child(child) { assert(_child != nullptr); }

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

Object::Ref<Widget> InheritedWidget::build(Object::Ref<BuildContext> context) { return _child; }

Object::Ref<Element> InheritedWidget::createElement()
{
    return Object::create<InheritedElement>(Object::cast<InheritedWidget>(this));
}

StatefulWidgetState::StatefulWidgetState() : mounted(false) {}

void StatefulWidgetState::initState() {}

void StatefulWidgetState::didWidgetUpdated(Object::Ref<StatefulWidget> oldWidget) {}

void StatefulWidgetState::didDependenceChanged() {}

void StatefulWidgetState::dispose() {}

void StatefulWidgetState::setState() {}

Object::Ref<BuildContext> StatefulWidgetState::getContext()
{
    assert(this->mounted && "User can't access context before initState. You can try [didDependenceChanged] method to access context before first build. ");
    return this->element.lock();
}