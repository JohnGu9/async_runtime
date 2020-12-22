#include "framework/widgets/widget.h"

Widget::Widget(Object::Ref<Key> key) : _key(key) {}

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
