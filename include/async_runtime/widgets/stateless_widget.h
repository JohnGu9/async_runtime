#pragma once

#include "widget.h"

class StatelessWidget : public Widget
{
public:
    StatelessWidget(Object::Ref<Key> key = nullptr);
    virtual Object::Ref<Widget> build(Object::Ref<BuildContext> context) = 0;

protected:
    Object::Ref<Element> createElement() override;
};