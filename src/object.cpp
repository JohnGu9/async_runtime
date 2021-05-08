#include <typeinfo>

#include "async_runtime/object.h"
#include "async_runtime/basic/string.h"

Object::RuntimeType Object::runtimeType()
{
    return typeid(*this).hash_code();
}

ref<String> Object::toString()
{
    std::stringstream ss;
    this->toStringStream(ss);
    return ss.str();
}

void Object::toStringStream(std::ostream &os)
{
    os << "class<" << typeid(*this).name()
       << ">[" << (size_t)this << ']';
}

#include "async_runtime/widgets/widget.h"
#include "async_runtime/elements/root_element.h"
int runApp(ref<Widget> widget, bool withConsole)
{
#ifndef NDEBUG
    info_print("Debug mode on");
    info_print("Root widget is " << widget->toString());
#else
    printf("\033c");
    info_print("Debug mode off");
#endif

    info_print(font_wrapper(BOLDCYAN, "AsyncRuntime") << " start");

    ref<RootElement> root = Object::create<RootElement>(widget);
    root->attach();

    if (withConsole)
        root->_console();
    else
        root->_noConsole();

    root->detach();
    const auto &exitCode = root->exitCode;

    info_print(font_wrapper(BOLDCYAN, "AsyncRuntime") << " exited with code " << exitCode);
    return exitCode;
}
