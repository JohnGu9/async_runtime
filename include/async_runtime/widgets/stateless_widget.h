#pragma once
#include "widget.h"

class Element;
class BuildContext;
class StatelessWidget : public Widget
{
public:
    StatelessWidget(option<Key> key = nullptr) : Widget(key) {}
    virtual ref<Widget> build(ref<BuildContext> context) = 0;

protected:
    ref<Element> createElement() override;
};
