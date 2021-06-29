#include "async_runtime/elements/root_element.h"
#include "async_runtime/widgets/process.h"
#include "async_runtime/fundamental/logger.h"

ref<Process> Process::of(ref<BuildContext> context)
{
    return context->dependOnInheritedWidgetOfExactType<Process>().assertNotNull();
}

Process::Process(ref<Widget> child, ref<RootElement> root, option<Key> key)
    : InheritedWidget(child, key), command(root->_command->stream), _root(root) {}

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
        root->_consoleStop = true;
        root->_exit(exitCode);
    }
}