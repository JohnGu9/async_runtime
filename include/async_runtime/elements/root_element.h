#pragma once

#include "../fundamental/async.h"
#include "../fundamental/logger.h"
#include "single_child_element.h"
#include <atomic>

class GlobalKey;
class LoggerHandler;
class ThreadPool;
class RootElement : public SingleChildElement
{
    friend class RootWidget;
    class RootWrapper;

public:
    finalref<Fn<void()>> exit;
    finalref<LoggerHandler> cout;

    RootElement(ref<Widget> child, Function<void()> exit);

    void attach() override;
    void detach() override;
    void visitDescendant(Function<bool(ref<Element>)>) override;

protected:
    ref<Widget> _child;
    void build() override;
    void update(ref<Widget> newWidget) override;
    void notify(ref<Widget> newWidget) override;
    void visitAncestor(Function<bool(ref<Element>)>) override;
};
