#include "async_runtime/widgets/stateful_builder.h"

StatefulBuilder::StatefulBuilder(
    Function<ref<Widget>(ref<BuildContext> /*context*/, Function<void(Function<void()>)> /*setState*/)> builder,
    ref<Key> key) : builder(builder), StatefulWidget(key) {}

class _StatefulBuilderState : public State<StatefulBuilder>
{
public:
    ref<Widget> build(ref<BuildContext> context) override
    {
        return this->getWidget()->builder(context, [this](Function<void()> fn) { this->setState(fn); });
    }
};

ref<State<StatefulWidget>> StatefulBuilder::createState()
{
    return Object::create<_StatefulBuilderState>();
}