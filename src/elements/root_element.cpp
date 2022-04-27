#include "async_runtime/elements/root_element.h"
#include "async_runtime/fundamental/thread.h"

#include "async_runtime/widgets/key.h"
#include "async_runtime/widgets/logger_widget.h"
#include "async_runtime/widgets/root_widget.h"

class RootElement::RootWrapper : public StatelessWidget
{
    ref<Widget> child;
    RootElement *rootElement;

public:
    RootWrapper(ref<Widget> child, RootElement *rootElement, option<Key> key = nullptr)
        : StatelessWidget(key), child(child), rootElement(rootElement) {}

    ref<Widget> build(ref<BuildContext> context) override
    {
        return Object::create<RootWidget>(
            Object::create<Logger>(child, rootElement->cout),
            rootElement);
    }
};

class _StdoutLoggerHandler : public LoggerHandler
{
    ref<EventLoop> _loop;

    ref<Future<bool>> _write(ref<String> str, bool wrap)
    {
        lateref<EventLoop> current;
        if (EventLoop::runningEventLoop.isNotNull(current) && _loop == current)
        {
            // call on one event loop
            // no need to sync
            return Future<bool>::async([str, wrap]
                                       {
                std::cout << str;
                if (wrap) std::cout<< std::endl;
                return true; });
        }
        else
        {
            // call from another event loop
            EventLoop::runningEventLoop.assertNotNull();
            auto completer = Object::create<Completer<bool>>(current);
            _loop->callSoonThreadSafe([str, current, completer, wrap]
                                      {
                std::cout << str;
                if (wrap) std::cout<< std::endl;
                current->callSoonThreadSafe([completer]{completer->complete(true);}); });
            return completer;
        }
    }

public:
    _StdoutLoggerHandler() : _loop(EventLoopGetterMixin::ensureEventLoop(nullptr)) {}

    ref<Future<bool>> write(ref<String> str) override { return _write(str, false); }
    ref<Future<bool>> writeLine(ref<String> str) override { return _write(str, true); }
    void dispose() override {}
};

static ref<LoggerHandler> coutLoggerBuilder()
{
    static finalref<LoggerHandler> singleton = Object::create<_StdoutLoggerHandler>();
    return singleton;
}

/**
 * @brief Construct a new Root Element:: Root Element object
 *
 * @param widget
 */
RootElement::RootElement(ref<Widget> widget, Function<void()> exit)
    : SingleChildElement(widget),
      exit(exit),
      cout(coutLoggerBuilder()),
      _child(Object::create<RootWrapper>(widget, this)) {}

void RootElement::update(ref<Widget> newWidget) { assert(false && "RootElement should never change. "); }

void RootElement::notify(ref<Widget> newWidget) { assert(false && "RootElement dependence would never change. "); }

void RootElement::attach()
{
    static finalref<Map<Object::RuntimeType, lateref<InheritedWidget>>> empty = Object::create<Map<Object::RuntimeType, lateref<InheritedWidget>>>();
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
