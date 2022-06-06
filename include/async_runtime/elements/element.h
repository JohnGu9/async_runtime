#pragma once

#include "build_context.h"

class Widget;
class Element : public BuildContext
{
    void initKey(ref<Widget>);
    static void disposeKey(ref<Widget>);

public:
    Element(ref<Widget> widget);
    weakref<Element> parent;

    virtual void attach();
    virtual void detach();
    virtual void build() = 0;

    virtual void update(ref<Widget> newWidget);
    virtual void notify(ref<Widget> newWidget);

    virtual void visitDescendant(Function<bool(ref<Element>)>) = 0;
    virtual void visitAncestor(Function<bool(ref<Element>)>);

    virtual const ref<Widget> &getWidget() const { return widget; }
};

#ifndef NDEBUG
#include "../utilities/console_format.h"
#endif

template <typename T, typename Object::Base<InheritedWidget>::isBaseOf<T>::type *>
option<T> BuildContext::dependOnInheritedWidgetOfExactType()
{
    auto iter = this->_inheritances->find(typeid(T).hash_code());
    if (iter == this->_inheritances.end())
    {
#ifndef NDEBUG
        this->cast<Element>()
            .ifNotNull([&](ref<Element> element) { //
                std::stringstream ss;
                ss << " > " << element->widget->toString() << std::endl;
                auto list = Object::create<List<ref<Widget>>>();
                element->visitAncestor([&list](ref<Element> element) { //
                    list->emplace_back(element->widget);
                    return false;
                });
                list->pop_back();
                list->forEach([&ss](ref<Widget> widget)
                              { ss << " > " << widget->toString() << std::endl; });
                debug_print("Can't find InheritedWidget [" << typeid(T).name() << "] from context: " << std::endl
                                                           << ss.rdbuf());
            });
#endif
        return nullptr;
    }
    return iter->second->cast<T>();
}
