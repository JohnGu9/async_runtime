#include "async_runtime/widgets/widget.h"
#include "async_runtime/widgets/key.h"

Widget::Widget(option<Key> key) : _key(key) {}

static bool keyEqual(const option<Key> &object0, const option<Key> &object1)
{
    lateref<Key> key;
    if_not_null(object0) return object0->isEqual(object1);
    else_if_not_null(object1) return object1->isEqual(object0);
    end_if() return true; // both are nullptr
}

bool Widget::canUpdate(ref<Widget> other)
{
    return this->runtimeType() == other->runtimeType() && ::keyEqual(this->key, other->key);
}
