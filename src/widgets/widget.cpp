#include "async_runtime/widgets/widget.h"
#include "async_runtime/widgets/key.h"

Widget::Widget(option<Key> key) : _key(key) {}

static bool equal(const option<Key> &object0, const option<Key> &object1)
{
    lateref<Key> key;
    if (object0.isNotNull(key))
    {
        return key->equal(object1);
    }
    else if (object1.isNotNull(key))
    {
        return key->equal(object0);
    }
    return true; // both are nullptr
}

bool Widget::canUpdate(ref<Widget> other)
{
    return this->runtimeType() == other->runtimeType() && equal(this->key, other->key);
}
