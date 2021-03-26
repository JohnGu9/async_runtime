#pragma once

#include "../basic/function.h"
#include "stateless_widget.h"

class Builder : public StatelessWidget
{
public:
    Builder(Function<ref<Widget>(ref<BuildContext>)> fn, option<Key> key = nullptr);
    virtual ref<Widget> build(ref<BuildContext> context) override;

protected:
    Function<ref<Widget>(ref<BuildContext>)> _fn;
};