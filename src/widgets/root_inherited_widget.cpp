#include "async_runtime/widgets/root_inherited_widget.h"
#include "async_runtime/fundamental/logger.h"
#include <sstream>

Object::Ref<RootInheritedWidget> RootInheritedWidget::of(Object::Ref<BuildContext> context)
{
    return context->dependOnInheritedWidgetOfExactType<RootInheritedWidget>();
}

bool RootInheritedWidget::updateShouldNotify(Object::Ref<InheritedWidget> oldWidget)
{
    if (Object::Ref<RootInheritedWidget> old = oldWidget->cast<RootInheritedWidget>())
        return old->_root.lock() != this->_root.lock();
    else
    {
        assert(false);
        return true;
    }
}

void RootInheritedWidget::requestExit()
{
    if (Object::Ref<RootElement> root = this->_root.lock())
        root->getMainHandler()->post([root] {
            root->_consoleStop = true;
            root->_condition.notify_all();
        });
    else
        throw "Missing [RootElement]";
}