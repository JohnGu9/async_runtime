#include "async_runtime/elements/root_element.h"
#include "async_runtime/widgets/root_inherited_widget.h"
#include "async_runtime/fundamental/logger.h"

ref<RootInheritedWidget> RootInheritedWidget::of(ref<BuildContext> context)
{
    return context->dependOnInheritedWidgetOfExactType<RootInheritedWidget>().assertNotNull();
}

bool RootInheritedWidget::updateShouldNotify(ref<InheritedWidget> oldWidget)
{
    ref<RootInheritedWidget> old = oldWidget->covariant<RootInheritedWidget>();
    return old->_root.lock() != this->_root.lock();
}

void RootInheritedWidget::exit()
{
    ref<RootElement> root = option<RootElement>(this->_root.lock()).assertNotNull();
    root->getMainHandler()->post([root] {
        root->_consoleStop = true;
        root->_condition.notify_all();
    });
}