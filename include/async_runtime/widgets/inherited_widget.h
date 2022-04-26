#pragma once

#include "stateless_widget.h"

class InheritedWidget : public StatelessWidget
{
public:
    InheritedWidget(ref<Widget> child, option<Key> key = nullptr);
    virtual bool updateShouldNotify(ref<InheritedWidget> oldWidget) = 0;
    ref<Widget> build(ref<BuildContext> context) override;
    ref<Element> createElement() override;

protected:
    finalref<Widget> _child;
};

#ifndef NDEBUG
#include "../elements/element.h"
#endif

template <typename T, typename std::enable_if<std::is_base_of<InheritedWidget, T>::value>::type *>
inline option<T> BuildContext::dependOnInheritedWidgetOfExactType()
{
    auto iter = this->_inheritances->find(typeid(T).hash_code());
    if (iter == this->_inheritances.end())
    {
#ifndef NDEBUG
        lateref<Element> element;
        if (option<Element>(this->cast<Element>()).isNotNull(element))
        {
            std::stringstream ss;
            ss << element->widget->toString();
            element->visitAncestor([&ss](ref<Element> element) {
                ss << " > " << element->widget->toString();
                return false;
            });
            debug_print("Can't find InheritedWidget [" << typeid(T).name() << "] from context: " << ss.rdbuf());
        }
#endif
        return nullptr;
    }
    return iter->second->cast<T>();
}
