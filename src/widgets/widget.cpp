#include "async_runtime/widgets/widget.h"

Widget::Widget(Object::Ref<Key> key) : _key(key) {}

bool Widget::canUpdate(Object::Ref<Widget> other)
{
    if (other == nullptr)
        return false;
    return this->runtimeType() == other->runtimeType() && this->key == other->key;
}

