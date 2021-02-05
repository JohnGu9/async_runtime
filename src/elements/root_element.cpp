#include "async_runtime/fundamental/logger.h"
#include "async_runtime/fundamental/scheduler.h"
#include "async_runtime/elements/element.h"
#include "async_runtime/elements/key.h"
#include "async_runtime/widgets/root_inherited_widget.h"
#include "async_runtime/basic/tree.h"

static inline void shutdownInfo()
{
    info_print(font_wrapper(BOLDCYAN, "AsyncRuntime") << " is shutting down");
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
    this->_child = Object::create<RootInheritedWidget>(this->_child, Object::cast<>(this));
    this->_inheritances = {};
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

void RootElement::visitDescendant(Function<bool(Object::Ref<Element>)> fn)
{
    assert(this->_childElement != nullptr);
    if (fn(this->_childElement) == false)
        this->_childElement->visitDescendant(fn);
}

void RootElement::visitAncestor(Function<bool(Object::Ref<Element>)>) {}

Logger::Handler RootElement::getStdoutHandler()
{
    if (Object::Ref<State<StatefulWidget>> currentState = this->_stdoutKey->getCurrentState())
        if (Object::Ref<StdoutLoggerState> state = currentState->cast<StdoutLoggerState>())
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
    Thread thread;
    {
        std::unique_lock<std::mutex> lock(this->_mutex);
        {
            auto self = Object::cast<>(this);
            thread = Thread([self] {
                self->_console();
                shutdownInfo();
                self->_condition.notify_all();
            });
        }
        this->_condition.wait(lock); // wait for exit
    }
    thread.detach();
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
            onCommand(input);
    }
}

void RootElement::onCommand(const std::string &in)
{
    Logger::Handler handler = this->getStdoutHandler();
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

    std::string::size_type commandLength = in.find(" ");
    std::string command = in.substr(0, commandLength == std::string::npos ? in.size() : commandLength);
    if (command == "clear")
    {
        printf("\033c");
    }
    else if (command == "ls")
    {
        Map<Element *, List<Element *>> map = {{this, List<Element *>::empty()}};
        this->visitDescendant([&map](Object::Ref<Element> element) -> bool {
            Object::Ref<Element> parent = element->parent.lock();
            map[parent.get()]->push_back(element.get());
            map[element.get()] = List<Element *>::empty();
            return false;
        });
        Object::Ref<Tree> tree = Object::create<Tree>();
        Function<void(Element *, Object::Ref<Tree>)> buildTree;
        buildTree =
            [&](Element *currentElement, Object::Ref<Tree> currentTree) {
                std::stringstream ss;
                ss << font_wrapper(BOLDBLUE, currentElement->runtimeType()) << " [" << (size_t)currentElement << "] " << std::endl
                   << "  widget: " << currentElement->widget->runtimeType() << std::endl;
                if (StatefulElement *statefulElement = dynamic_cast<StatefulElement *>(currentElement))
                    ss << "  state: " << statefulElement->_state->runtimeType() << " [" << (size_t)statefulElement->_state.get() << "] " << std::endl;
                currentTree->info = ss.str();
                List<Element *> &children = map[currentElement];
                for (Element *child : children)
                {
                    Object::Ref<Tree> childTree = Object::create<Tree>();
                    buildTree(child, childTree);
                    currentTree->children->push_back(childTree);
                }
            };
        buildTree(this, tree);
        this->getMainHandler()->post([&] { tree->toStringStream(std::cout); }).get();
    }
    else if (command == "ps")
    {
    }
    else if (command == "reassembly")
    {
        this->detach();
        this->attach();
    }
    else
    {
        error_print("No such method");
        info_print("'-h' or '--help' for more information");
    }
}