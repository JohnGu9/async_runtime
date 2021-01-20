#include "async_runtime/widgets/builder.h"

Builder::Builder(Fn<Object::Ref<Widget>(Object::Ref<BuildContext>)> fn, Object::Ref<Key> key) : _fn(fn), StatelessWidget(key) {}

Object::Ref<Widget> Builder::build(Object::Ref<BuildContext> context)
{
    return _fn(context);
}