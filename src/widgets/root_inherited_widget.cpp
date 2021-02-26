#include "async_runtime/elements/root_element.h"
#include "async_runtime/widgets/root_inherited_widget.h"
#include "async_runtime/fundamental/logger.h"

ref<RootInheritedWidget> RootInheritedWidget::of(ref<BuildContext> context)
{
    return context->dependOnInheritedWidgetOfExactType<RootInheritedWidget>();
}

bool RootInheritedWidget::updateShouldNotify(ref<InheritedWidget> oldWidget)
{
    if (ref<RootInheritedWidget> old = oldWidget->cast<RootInheritedWidget>())
        return old->_root.lock() != this->_root.lock();
    else
    {
        assert(false);
        return true;
    }
}

void RootInheritedWidget::requestExit()
{
    if (ref<RootElement> root = this->_root.lock())
        root->getMainHandler()->post([root] {
            root->_consoleStop = true;
            root->_condition.notify_all();
        });
    else
        throw "Missing [RootElement]";
}