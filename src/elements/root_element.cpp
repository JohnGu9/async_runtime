#include "async_runtime/basic/tree.h"
#include "async_runtime/fundamental/logger.h"
#include "async_runtime/elements/root_element.h"
#include "async_runtime/elements/key.h"
#include "async_runtime/widgets/stateful_widget.h"
#include "async_runtime/widgets/process.h"
#include "async_runtime/widgets/scheduler.h"

class RootElement::RootWidget : public Widget
{
public:
    RootWidget() : Widget(nullptr) {}
    ref<Element> createElement() override { return Object::create<RootElement>(self()); }
};

class RootElement::RootFundamental::_State : public State<RootFundamental>
{
    using super = State<RootFundamental>;

    void initState() override
    {
        super::initState();
        widget->rootElement->_coutKey = Object::create<GlobalKey>();
        widget->rootElement->_command = Object::create<AsyncStreamController<ref<String>>>(self());
    }

    void dispose() override
    {
        widget->rootElement->_command->close();
        super::dispose();
    }

    ref<Widget> build(ref<BuildContext>) override
    {
        return Object::create<Process>(
            Object::create<StdoutLogger>(widget->child, widget->rootElement->_coutKey), // StdoutLogger
            Object::cast<>(widget->rootElement));                                       // Process
    }
};

ref<State<StatefulWidget>> RootElement::RootFundamental::createState()
{
    return Object::create<_State>();
}

/**
 * @brief Construct a new Root Element:: Root Element object
 * 
 * @param widget 
 */
RootElement::RootElement(ref<Widget> widget)
    : SingleChildElement(Object::create<RootWidget>()),
      _child(Object::create<Scheduler>(Object::create<RootFundamental>(widget, this))), _consoleStop(false) {}

void RootElement::update(ref<Widget> newWidget) { assert(false && "RootElement should never change. "); }

void RootElement::notify(ref<Widget> newWidget) { assert(false && "RootElement dependence would never change. "); }

void RootElement::attach()
{
    static finalref<Map<Object::RuntimeType, lateref<Inheritance>>> empty = Object::create<Map<Object::RuntimeType, lateref<Inheritance>>>();
    this->_inheritances = empty;
    this->attachElement(this->_child->createElement());
}

void RootElement::build()
{
    ref<Widget> widget = this->_child;
    ref<Widget> lastWidget = this->_childElement->getWidget();
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

void RootElement::visitDescendant(Function<bool(ref<Element>)> fn)
{
    if (fn(this->_childElement) == false)
        this->_childElement->visitDescendant(fn);
}

void RootElement::visitAncestor(Function<bool(ref<Element>)>) {}

void RootElement::_exit(int exitCode)
{
    this->_exitCode = exitCode;
    return this->_condition.notify_all();
}

Logger::Handler RootElement::getStdoutHandler()
{
    ref<State<StatefulWidget>> currentState = this->_coutKey->getCurrentState().assertNotNull();
    ref<StdoutLoggerState> state = currentState->covariant<StdoutLoggerState>();
    return state->_handler;
}

Scheduler::Handler RootElement::getMainHandler()
{
    ref<BuildContext> context = this->_coutKey->getCurrentContext().assertNotNull();
    return Scheduler::of(context);
}

void RootElement::_console()
{
    std::unique_lock<std::mutex> lock(this->_mutex);
    auto self = self();
    auto thread = Thread([this, self]
                         {
#ifndef NDEBUG
                             ThreadPool::setThreadName("ConsoleThread");
#endif
                             info_print("Enter '"
                                        << font_wrapper(BOLDBLUE, 'q')
                                        << "' to quit, '"
                                        << font_wrapper(BOLDBLUE, "-h")
                                        << "' or '"
                                        << font_wrapper(BOLDBLUE, "--help")
                                        << "' for more information");

                             while (true)
                             {
                                 this->getStdoutHandler()->write(">> ")->sync();
                                 ref<String> input = getline(std::cin);
                                 if (this->_consoleStop) // runApp already required to exit, console not more accept command
                                     return;
                                 if (input == "q" || input == "quit")
                                 {
                                     std::stringstream ss;
                                     ss << "Sure to quit (" << font_wrapper(BOLDBLUE, 'y') << '/' << font_wrapper(BOLDRED, "n") << " default is n)? ";
                                     this->getStdoutHandler()->write(ss.str())->sync();
                                     std::string confrim;
                                     if (std::getline(std::cin, confrim) && (confrim == "y" || confrim == "yes" || this->_consoleStop))
                                         break;
                                     this->getStdoutHandler()->writeLine("cancel")->sync();
                                 }
                                 else
                                     onCommand(input);
                             }
                             if (this->_consoleStop == false)
                             {
                                 // runApp exit by console command
                                 info_print(font_wrapper(BOLDCYAN, "AsyncRuntime") << " is shutting down");
                                 this->_consoleStop = true;
                                 this->_exit(0);
                             }
                         });
    this->_condition.wait(lock); // wait for exit
    thread.detach();
}

void RootElement::_noConsole()
{
    std::unique_lock<std::mutex> lock(this->_mutex);
    this->_condition.wait(lock);
}

void RootElement::onCommand(const ref<String> &in)
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
        ss << help_format("reassembly", "()") << std::endl;
        ss << help_format("clear\t", "()") << std::endl;
        handler->write(ss.str());
        return;
    }
    else if (in->isEmpty())
        return;

    std::string::size_type commandLength = in->find(" ");
    auto command = in->substr(0, commandLength == String::npos ? in->size() : commandLength);
    if (command == "clear")
    {
        printf("\033c");
    }
    else if (command == "ls")
    {
        ref<Map<Element *, lateref<List<Element *>>>> map = {{this, Object::create<List<Element *>>()}};
        this->visitDescendant([&map](ref<Element> element) -> bool
                              {
                                  option<Element> parent = element->parent.toOption();
                                  map[parent.get()]->emplace_back(element.get());
                                  map[element.get()] = Object::create<List<Element *>>();
                                  return false;
                              });
        ref<Tree> tree = Object::create<Tree>();
        lateref<Fn<void(Element *, ref<Tree>)>> buildTree;
        buildTree =
            [&](Element *currentElement, ref<Tree> currentTree)
        {
            std::stringstream ss;
            ss << font_wrapper(BOLDBLUE, currentElement->toString()) << std::endl
               << "  widget: " << currentElement->getWidget()->toString() << std::endl;
            if (StatefulElement *statefulElement = dynamic_cast<StatefulElement *>(currentElement))
                ss << "  state: " << statefulElement->_state->toString() << std::endl;
            currentTree->info = ss.str();
            ref<List<Element *>> &children = map[currentElement];
            for (Element *child : children)
            {
                ref<Tree> childTree = Object::create<Tree>();
                buildTree(child, childTree);
                currentTree->children->emplace_back(childTree);
            }
        };
        buildTree(this, tree);
        this->getMainHandler()->post([&]
                                     { tree->toStringStream(std::cout); })
            .get();
    }
    else if (command == "reassembly")
    {
        this->detach();
        this->attach();
    }
    else
    {
        if ((command == "command" || command == "cmd") && in->length() > commandLength)
        {
            auto begin = in->find_first_not_of(" ", commandLength);
            if (begin != String::npos)
            {
                auto cmd = in->substr(begin);
                this->_command->sinkSync(cmd);
                return;
            }
        }
        error_print("No such method");
        info_print("'-h' or '--help' for more information");
    }
}