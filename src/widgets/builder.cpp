#include "async_runtime/widgets/builder.h"

Builder::Builder(Function<ref<Widget>(ref<BuildContext>)> fn, option<Key> key) : StatelessWidget(key), _fn(fn) {}

ref<Widget> Builder::build(ref<BuildContext> context)
{
    return _fn(context);
}