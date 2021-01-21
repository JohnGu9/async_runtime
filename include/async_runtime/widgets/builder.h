#pragma once

#include "../basic/function.h"
#include "stateless_widget.h"

class Builder : public StatelessWidget
{
public:
    Builder(Function<Object::Ref<Widget>(Object::Ref<BuildContext>)> fn, Object::Ref<Key> key = nullptr);
    virtual Object::Ref<Widget> build(Object::Ref<BuildContext> context) override;

protected:
    Function<Object::Ref<Widget>(Object::Ref<BuildContext>)> _fn;
};