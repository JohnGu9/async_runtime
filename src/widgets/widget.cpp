#include "async_runtime/widgets/widget.h"

Widget::Widget(ref<Key> key) : _key(key) {}

bool Widget::canUpdate(ref<Widget> other)
{
    if (other == nullptr)
        return false;
    return this->runtimeType() == other->runtimeType() && this->key == other->key;
}

