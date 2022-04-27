#pragma once
#include "../fundamental/logger.h"
#include "inherited_widget.h"

class RootElement;
class RootWidget : public InheritedWidget
{
public:
    static ref<RootWidget> of(ref<BuildContext> context);
    finalref<Fn<void()>> &exit;
    finalref<LoggerHandler> &cout;

    RootWidget(ref<Widget> child, RootElement *element, option<Key> key = nullptr);
    bool updateShouldNotify(ref<InheritedWidget> oldWidget) override { return false; }

protected:
    RootElement *element;
};