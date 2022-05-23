#include "async_runtime/widgets/stateful_widget.h"
#include "async_runtime/elements/stateful_element.h"

void State<StatefulWidget>::initState()
{
#ifndef NDEBUG
    assert(_mounted == false && "This [State] class mount twice is not allowed. User should not reuse [State] class or manually call [initState]");
#endif
    _mounted = true;
}

void State<StatefulWidget>::dispose()
{
#ifndef NDEBUG
    assert(_mounted == true && "This [State] class dispose more then twice is not allowed. User should not reuse [State] class or manually call [dispose]");
#endif
    _mounted = false;
    static finalref<List<Function<void()>>> dummy = List<Function<void()>>::create();
    _setStateCallbacks = dummy;
}

void State<StatefulWidget>::beforeBuild()
{
    for (const auto &fn : this->_setStateCallbacks)
        fn();
    this->_setStateCallbacks->clear(); // state no longer dirty
}

void State<StatefulWidget>::setState(Function<void()> fn)
{
#ifndef NDEBUG
    assert(this->_loop == EventLoop::runningEventLoop && "[setState] can't call in other event loops");
    assert(this->_mounted && "[setState] call on a disposed State");
    assert(this->_element->_lifeCycle == StatefulElement::LifeCycle::mounted && "[setState] can't not call in widget lifecycle hooker. ");
#endif
    auto self = self();
    this->_setStateCallbacks->emplace_back(fn); // mark state dirty
    this->_loop->callSoon([this, self] {        //
        if (!this->_setStateCallbacks->empty()) // if state is dirty, rebuild the tree
            this->_element->build();
    });
}

ref<Element> StatefulWidget::createElement()
{
    return Object::create<StatefulElement>(self());
}
