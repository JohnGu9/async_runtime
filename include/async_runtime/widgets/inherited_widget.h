#pragma once

#include "stateless_widget.h"

class InheritedWidget : public StatelessWidget, public Inheritance
{
public:
    InheritedWidget(ref<Widget> child, ref<Key> key = nullptr);
    virtual bool updateShouldNotify(ref<InheritedWidget> oldWidget) = 0;
    ref<Widget> build(ref<BuildContext> context) override;
    ref<Element> createElement() override;

protected:
    ref<Widget> _child;
};

template <typename T, typename std::enable_if<std::is_base_of<InheritedWidget, T>::value>::type *>
inline ref<T> BuildContext::dependOnInheritedWidgetOfExactType()
{
    auto iter = this->_inheritances->find(typeid(T).name());
    if (iter == this->_inheritances.end())
    {
#ifdef DEBUG
        if (ref<Element> element = this->cast<Element>())
        {
            String str = element->widget->runtimeType();
            element->visitAncestor([&str](ref<Element> element) {
                str = str + " > " + element->widget->runtimeType();
                return false;
            });
            debug_print("Can't find InheritedWidget [" << typeid(T).name() << "] from context: " << str);
        }
#endif
        return nullptr;
    }
    return iter->second->cast<T>();
}
