#include "framework/fundamental/scheduler.h"

Scheduler::Handler Scheduler::of(Object::Ref<BuildContext> context)
{
    return context->dependOnInheritanceOfExactType<SchedulerElement>()->_handler;
}

Scheduler::Scheduler(Object::Ref<Widget> child, Object::Ref<Key> key)
    : _child(child), StatelessWidget(key) {}

Object::Ref<Widget> Scheduler::build(Object::Ref<BuildContext> context)
{
    return this->_child;
}

Object::Ref<Element> Scheduler::createElement()
{
    return Object::create<SchedulerElement>(Object::cast<Scheduler>(this));
}

SchedulerElement::SchedulerElement(Object::Ref<Scheduler> widget) : StatelessElement(widget) {}

void SchedulerElement::attach()
{
    Element::attach();
    this->_handler = Object::create<ThreadPool>(1);
    this->_handler->post([this] {
            this->_inheritances[this->runtimeType()] = Object::cast<SchedulerElement>(this);
            Object::Ref<Widget> widget = this->_statelessWidget->build(Object::cast<BuildContext>(this));
            assert(widget != nullptr && "InheritedWidget child should not return null, make sure call InheritedWidget init constructer. ");
            this->attachElement(widget->createElement()); }).get();
}

void SchedulerElement::detach()
{
    this->_handler->post([this] {
            this->detachElement();
            this->_statelessWidget = nullptr; }).get();
    this->_handler->dispose();
    Element::detach();
}

void SchedulerElement::build()
{
    this->_handler->post([this] { this->toBuild(); }).get();
}

void SchedulerElement::notify(Object::Ref<Widget> newWidget)
{
    this->_handler->post([this, newWidget] { this->toNotify(newWidget); }).get();
}

void SchedulerElement::update(Object::Ref<Widget> newWidget)
{
    this->_handler->post([this, newWidget] { this->toUpdate(newWidget); }).get();
}

void SchedulerElement::toBuild() { StatelessElement::build(); }

void SchedulerElement::toNotify(Object::Ref<Widget> newWidget)
{
    Element::notify(newWidget);
    this->_inheritances[this->runtimeType()] = Object::cast<SchedulerElement>(this); // update inheritances
    this->_statelessWidget = newWidget->cast<StatelessWidget>();
    assert(this->_childElement != nullptr);
    assert(this->_statelessWidget);
    Object::Ref<Widget> widget = this->_statelessWidget->build(Object::cast<BuildContext>(this));
    assert(widget != nullptr && "StatelessWidget build method should not return null. Try to return a [LeafWidget] to end the build tree. ");
    Object::Ref<Widget> oldWidget = this->_childElement->widget;
    if (Object::identical(widget, oldWidget) || widget->canUpdate(oldWidget))
        this->_childElement->notify(widget);
    else
        this->reattachElement(widget->createElement());
}

void SchedulerElement::toUpdate(Object::Ref<Widget> newWidget) { StatelessElement::update(newWidget); }
