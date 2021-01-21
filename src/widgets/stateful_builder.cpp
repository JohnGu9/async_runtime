#include "async_runtime/widgets/stateful_builder.h"

StatefulBuilder::StatefulBuilder(
    Function<Object::Ref<Widget>(Object::Ref<BuildContext> /*context*/, Function<void(Function<void()>)> /*setState*/)> builder,
    Object::Ref<Key> key) : builder(builder), StatefulWidget(key) {}

class _StatefulBuilderState : public State<StatefulBuilder>
{
public:
    Object::Ref<Widget> build(Object::Ref<BuildContext> context) override
    {
        return this->getWidget()->builder(context, [this](Function<void()> fn) { this->setState(fn); });
    }
};

Object::Ref<State<StatefulWidget>> StatefulBuilder::createState()
{
    return Object::create<_StatefulBuilderState>();
}