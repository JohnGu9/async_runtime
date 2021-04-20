#include "async_runtime/elements/root_element.h"
#include "async_runtime/widgets/process.h"
#include "async_runtime/fundamental/logger.h"

ref<Process> Process::of(ref<BuildContext> context)
{
    return context->dependOnInheritedWidgetOfExactType<Process>().assertNotNull();
}

bool Process::updateShouldNotify(ref<InheritedWidget> oldWidget)
{
    ref<Process> old = oldWidget->covariant<Process>();
    return old->_root.toOption() != this->_root.toOption();
}

void Process::exit(const int exitCode)
{
    lateref<RootElement> root;
    if (_root.isNotNull(root))
    {
        root->getMainHandler()->post([root, exitCode] {
            root->_consoleStop = true;
            root->_exitCode = exitCode;
            root->_condition.notify_all();
        });
    }
}