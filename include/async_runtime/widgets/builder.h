#pragma once

#include "../basic/function.h"
#include "stateless_widget.h"

class Builder : public StatelessWidget
{
public:
    Builder(Fn<Object::Ref<Widget>(Object::Ref<BuildContext>)> fn, Object::Ref<Key> key = nullptr);
    virtual Object::Ref<Widget> build(Object::Ref<BuildContext> context) override;

protected:
    Fn<Object::Ref<Widget>(Object::Ref<BuildContext>)> _fn;
};