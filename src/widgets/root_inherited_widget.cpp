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
    return old->_root.toOption() != this->_root.toOption();
}

void RootInheritedWidget::exit()
{
    ref<RootElement> root = this->_root.assertNotNull();
    root->getMainHandler()->post([root] {
        root->_consoleStop = true;
        root->_condition.notify_all();
    });
}