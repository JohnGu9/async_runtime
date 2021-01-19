#include "framework/fundamental/logger.h"
#include "framework/elements/element.h"
#include "framework/widgets/key.h"
#include "framework/widgets/root_inherited_widget.h"

static void onCommand(const std::string &in, Object::Ref<RootElement> root)
{
    Logger::Handler handler = root->getStdoutHandler();
    if (in == "-h" || in == "--help")
    {
#define help_format(x, y) " " << x << "\t\t" << y
        std::stringstream ss;
        ss << std::endl;
        ss << help_format(BOLDWHITE << "command", "arguments" << RESET) << std::endl;
        ss << help_format("-------", "---------") << std::endl;
        ss << help_format("ls\t", "()") << std::endl;
        ss << help_format("ps\t", "()") << std::endl;
        ss << help_format("pwd\t", "()") << std::endl;
        ss << help_format("reassembly", "()") << std::endl;
        handler->write(ss.str());
        return;
    }
    else if (in.empty())
        return;

    auto commandLength = in.find(" ");
    auto command = in.substr(0, commandLength == std::string::npos ? in.size() : commandLength);
    if (command == "clear")
    {
        printf("\033c");
    }
    else if (command == "ls")
    {
        Object::Map<Object::RuntimeType, Object::List<size_t>> map;
        root->visitDescendant([&map](Object::Ref<Element> element) -> bool {
            Object::RuntimeType type = element->runtimeType();
            if (map.find(type) == map.end())
                map[type] = {};
            map[type].push_back((size_t)(element.get()));
            return false;
        });
        std::stringstream ss;
        for (auto iter = map.begin(); iter != map.end(); iter++)
        {
            ss << font_wrapper(BOLDGREEN, iter->first) << "[ ";
            for (auto i : iter->second)
                ss << i << " ";
            ss << "]  ";
        }
        handler->writeLine(ss.str());
    }
    else if (command == "ps")
    {
    }
    else if (command == "pwd")
    {
    }
    else if (command == "reassembly")
    {
        root->detach();
        root->attach();
    }
    else
    {
        error_print("No such method");
        info_print("'-h' or '--help' for more information");
    }
}

static inline void bootInfo()
{
    info_print(font_wrapper(BOLDCYAN, "AsyncRuntime") << " boot");
}

static inline void shutdownInfo()
{
    info_print(font_wrapper(BOLDCYAN, "AsyncRuntime") << " is shuting down");
}

struct _MockWidget : Widget
{
    _MockWidget() : Widget(nullptr) {}
    Object::Ref<Element> createElement() override { return nullptr; }
};

/// Root Element
RootElement::RootElement(Object::Ref<Widget> widget) : _consoleStop(false), SingleChildElement(nullptr)
{
    assert(widget != nullptr);
    this->_stdoutKey = Object::create<GlobalKey>();
    this->_child = Object::create<Scheduler>(Object::create<StdoutLogger>(widget, _stdoutKey));
}

void RootElement::update(Object::Ref<Widget> newWidget) { assert(false && "RootElement should never change. "); }

void RootElement::notify(Object::Ref<Widget> newWidget) { assert(false && "RootElement dependence would never change. "); }

void RootElement::attach()
{
    this->widget = Object::create<_MockWidget>();
    this->_child = Object::create<RootInheritedWidget>(this->_child, Object::self(this));
    this->attachElement(this->_child->createElement());
}

void RootElement::build()
{
    assert(this->_childElement != nullptr);
    Object::Ref<Widget> widget = this->_child;
    Object::Ref<Widget> lastWidget = this->_childElement->widget;
    if (Object::identical(widget, lastWidget))
        return;
    else if (widget->canUpdate(lastWidget))
        this->_childElement->update(widget);
    else
        this->reattachElement(widget->createElement());
}

void RootElement::detach()
{
    this->detachElement();
    Element::detach();
}

void RootElement::visitDescendant(Fn<bool(Object::Ref<Element>)> fn)
{
    assert(this->_childElement != nullptr);
    if (fn(this->_childElement) == false)
        this->_childElement->visitDescendant(fn);
}

void RootElement::visitAncestor(Fn<bool(Object::Ref<Element>)>) {}

Logger::Handler RootElement::getStdoutHandler()
{
    if (Object::Ref<StatefulWidgetState> currentState = this->_stdoutKey->getCurrentState())
        if (Object::Ref<_StdoutLoggerState> state = currentState->cast<_StdoutLoggerState>())
            return state->_handler;
    return nullptr;
}

Scheduler::Handler RootElement::getMainHandler()
{
    if (Object::Ref<BuildContext> context = this->_stdoutKey->getCurrentContext())
        return Scheduler::of(context);
    return nullptr;
}

void RootElement::scheduleRootWidget()
{
    this->attach();
    ThreadPool threadPool(1);
    {
        std::unique_lock<std::mutex> lock(this->_mutex);
        {
            auto self = Object::self(this);
            threadPool.post([self] {
                bootInfo();
                self->_console();
                shutdownInfo();
                self->_condition.notify_all();
            });
        }
        this->_condition.wait(lock); // wait for exit
    }
    threadPool.forceClose();
    this->detach();
}

void RootElement::_console()
{
    {
        std::stringstream ss;
        ss << "Enter '"
           << font_wrapper(BOLDBLUE, 'q')
           << "' to quit, '"
           << font_wrapper(BOLDBLUE, "-h")
           << "' or '"
           << font_wrapper(BOLDBLUE, "--help")
           << "' for more information";
        this->getStdoutHandler()->writeLine(ss.str());
    }

    std::string input;
    for (;;)
    {
        this->getStdoutHandler()->write(">> ").get();
        std::getline(std::cin, input);
        if (this->_consoleStop)
            return;
        if (input == "q" || input == "quit")
        {
            std::stringstream ss;
            ss << "Sure to quit (" << font_wrapper(BOLDBLUE, 'y') << '/' << font_wrapper(BOLDRED, "n") << " default is n)? ";
            this->getStdoutHandler()->write(ss.str()).get();
            if (std::getline(std::cin, input) && (input == "y" || input == "yes" || this->_consoleStop))
                return;
            this->getStdoutHandler()->writeLine("cancel").get();
        }
        else
            onCommand(input, Object::self(this));
    }
}