#pragma once

#include "../basic/function.h"
#include "../widgets/state.h"
#include "../contexts/build_context.h"
#include "../fundamental/async.h"

class Widget;
class LeafWidget;
class StatelessWidget;
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

    virtual void visitDescendant(Function<bool(Object::Ref<Element>)>) = 0;
    virtual void visitAncestor(Function<bool(Object::Ref<Element>)>);
};

class LeafElement : public Element
{
public:
    LeafElement(Object::Ref<LeafWidget> widget);
    void build() override;
    void update(Object::Ref<Widget> newWidget) override;
    void notify(Object::Ref<Widget> newWidget) override;

    void visitDescendant(Function<bool(Object::Ref<Element>)>) override;
};

class SingleChildElement : public Element
{
public:
    SingleChildElement(Object::Ref<Widget> widget);

protected:
    Object::Ref<Element> _childElement;
    void attachElement(Object::Ref<Element>);
    void reattachElement(Object::Ref<Element>);
    void detachElement();
};

class GlobalKey;
class LoggerHandler;
class RootInheritedWidget;
class RootElement : public SingleChildElement
{
    friend RootInheritedWidget;

public:
    RootElement(Object::Ref<Widget> child);

    void attach() override;
    void detach() override;
    void build() override;
    void update(Object::Ref<Widget> newWidget) override;
    void notify(Object::Ref<Widget> newWidget) override;

    void visitDescendant(Function<bool(Object::Ref<Element>)>) override;
    void visitAncestor(Function<bool(Object::Ref<Element>)>) override;

    virtual Object::Ref<LoggerHandler> getStdoutHandler();
    virtual Object::Ref<ThreadPool> getMainHandler();
    virtual void scheduleRootWidget();
    virtual void onCommand(const std::string &in);

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

    void attach() override;
    void detach() override;
    void build() override;
    void notify(Object::Ref<Widget> newWidget) override;
    void update(Object::Ref<Widget> newWidget) override;

    void visitDescendant(Function<bool(Object::Ref<Element>)>) override;
};

class StatefulElement : public SingleChildElement
{
    template <typename T, typename std::enable_if<std::is_base_of<StatefulWidget, T>::value>::type *>
    friend class State;

public:
    StatefulElement(Object::Ref<StatefulWidget> widget);

    Object::Ref<StatefulWidget> _statefulWidget;
    Object::Ref<State<StatefulWidget>> _state;

protected:
    class _LifeCycle
    {
    public:
        enum Value
        {
            uninitialized,
            mounted,
            building,
            unmount,
        };
        static const List<Value> values;
        static String toString(Value value);
    };

    _LifeCycle::Value _lifeCycle;

    void attach() override;
    void detach() override;
    void build() override;
    void notify(Object::Ref<Widget> newWidget) override;
    void update(Object::Ref<Widget> newWidget) override;

    void visitDescendant(Function<bool(Object::Ref<Element>)>) override;
};

class InheritedElement : public SingleChildElement
{
public:
    InheritedElement(Object::Ref<InheritedWidget> child);
    Object::Ref<InheritedWidget> _inheritWidget;

    void attach() override;
    void detach() override;
    void build() override;
    void update(Object::Ref<Widget> newWidget) override;
    void notify(Object::Ref<Widget> newWidget) override;

    void visitDescendant(Function<bool(Object::Ref<Element>)>) override;
};

class MultiChildElement : public Element
{
public:
    MultiChildElement(Object::Ref<MultiChildWidget> widget);
    void attach() override;
    void detach() override;
    void build() override;
    void update(Object::Ref<Widget> newWidget) override;
    void notify(Object::Ref<Widget> newWidget) override;

    void visitDescendant(Function<bool(Object::Ref<Element>)>) override;

protected:
    Object::Ref<MultiChildWidget> _multiChildWidget;
    List<Object::Ref<Element>> _childrenElements;
};

class NotificationListenerElement : public StatelessElement
{
public:
    NotificationListenerElement(Object::Ref<NotificationListener> child);
    Object::Ref<NotificationListener> _notificationListenerWidget;

    void detach() override;
    void update(Object::Ref<Widget> newWidget) override;
    void notify(Object::Ref<Widget> newWidget) override;
};