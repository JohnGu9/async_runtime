#include "async_runtime/widgets/widget.h"

Widget::Widget(option<Key> key) : _key(key) {}

bool Widget::canUpdate(ref<Widget> other)
{
    return this->runtimeType() == other->runtimeType() && this->key == other->key;
}
