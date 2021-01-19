#pragma once

#include "../basic/function.h"
#include "../contexts/build_context.h"
#include "../fundamental/thread_pool.h"

class Widget;
class LeafWidget;
class StatelessWidget;
class StatefulWidget;
class StatefulWidgetState;
class InheritedWidget;
class NotificationListener;
class MultiChildWidget;

class Element : public BuildContext
{
public:
    Element(Object::Ref<Widget> widget);
    Object::WeakRef<Element> parent;

    virtual void attach();
    virtual void detach();
    virtual void build() = 0;

    virtual void update(Object::Ref<Widget> newWidget);
    virtual void notify(Object::Ref<Widget> newWidget);

    virtual void visitDescendant(Fn<bool(Object::Ref<Element>)>) = 0;
    virtual void visitAncestor(Fn<bool(Object::Ref<Element>)>);
};

class LeafElement : public Element
{
public:
    LeafElement(Object::Ref<LeafWidget> widget);
    virtual void build() override;
    virtual void update(Object::Ref<Widget> newWidget) override;
    virtual void notify(Object::Ref<Widget> newWidget) override;

    virtual void visitDescendant(Fn<bool(Object::Ref<Element>)>) override;
};

class SingleChildElement : public Element
{
public:
    SingleChildElement(Object::Ref<Widget> widget);

protected:
    Object::Ref<Element> _childElement;
    virtual void attachElement(Object::Ref<Element>);
    virtual void reattachElement(Object::Ref<Element>);
    virtual void detachElement();
};

class GlobalKey;
class LoggerHandler;
class RootInheritedWidget;
class RootElement : public SingleChildElement
{
    friend RootInheritedWidget;

public:
    RootElement(Object::Ref<Widget> child);

    virtual void attach() override;
    virtual void detach() override;
    virtual void build() override;
    virtual void update(Object::Ref<Widget> newWidget) override;
    virtual void notify(Object::Ref<Widget> newWidget) override;

    virtual void visitDescendant(Fn<bool(Object::Ref<Element>)>) override;
    virtual void visitAncestor(Fn<bool(Object::Ref<Element>)>) override;

    virtual Object::Ref<LoggerHandler> getStdoutHandler();
    virtual Object::Ref<ThreadPool> getMainHandler();
    virtual void scheduleRootWidget();

protected:
    std::mutex _mutex;
    std::condition_variable _condition;

    Object::Ref<Widget> _child;
    Object::Ref<GlobalKey> _stdoutKey;
    std::atomic_bool _consoleStop;
    virtual void _console();
};

class StatelessElement : public SingleChildElement
{
public:
    StatelessElement(Object::Ref<StatelessWidget> widget);

protected:
    Object::Ref<StatelessWidget> _statelessWidget;

    virtual void attach() override;
    virtual void detach() override;
    virtual void build() override;
    virtual void notify(Object::Ref<Widget> newWidget) override;
    virtual void update(Object::Ref<Widget> newWidget) override;

    virtual void visitDescendant(Fn<bool(Object::Ref<Element>)>) override;
};

class StatefulElement : public SingleChildElement
{
    friend StatefulWidgetState;

public:
    StatefulElement(Object::Ref<StatefulWidget> widget);

    Object::Ref<StatefulWidget> _statefulWidget;
    Object::Ref<StatefulWidgetState> _state;

protected:
    class LifeCycle
    {
    public:
        enum Enum
        {
            uninitialized,
            mounted,
            building,
            unmount,
        };
    };
    LifeCycle::Enum _lifeCycle;

    virtual void attach() override;
    virtual void detach() override;
    virtual void build() override;
    virtual void notify(Object::Ref<Widget> newWidget) override;
    virtual void update(Object::Ref<Widget> newWidget) override;

    virtual void visitDescendant(Fn<bool(Object::Ref<Element>)>) override;
};

class InheritedElement : public SingleChildElement
{
public:
    InheritedElement(Object::Ref<InheritedWidget> child);
    Object::Ref<InheritedWidget> _inheritWidget;

    virtual void attach() override;
    virtual void detach() override;
    virtual void build() override;
    virtual void update(Object::Ref<Widget> newWidget) override;
    virtual void notify(Object::Ref<Widget> newWidget) override;

    virtual void visitDescendant(Fn<bool(Object::Ref<Element>)>) override;
};

class MultiChildElement : public Element
{
public:
    MultiChildElement(Object::Ref<MultiChildWidget> widget);
    virtual void attach() override;
    virtual void detach() override;
    virtual void build() override;
    virtual void update(Object::Ref<Widget> newWidget) override;
    virtual void notify(Object::Ref<Widget> newWidget) override;

    virtual void visitDescendant(Fn<bool(Object::Ref<Element>)>) override;

protected:
    Object::Ref<MultiChildWidget> _multiChildWidget;
    Object::List<Object::Ref<Element>> _childrenElements;
};

class NotificationListenerElement : public StatelessElement
{
public:
    NotificationListenerElement(Object::Ref<NotificationListener> child);
    Object::Ref<NotificationListener> _notificationListenerWidget;

    virtual void detach() override;
    virtual void update(Object::Ref<Widget> newWidget) override;
    virtual void notify(Object::Ref<Widget> newWidget) override;
};