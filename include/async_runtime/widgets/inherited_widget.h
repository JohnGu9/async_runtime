#pragma once

#include "stateless_widget.h"

class InheritedWidget : public StatelessWidget, public Inheritance
{
public:
    InheritedWidget(Object::Ref<Widget> child, Object::Ref<Key> key = nullptr);
    virtual bool updateShouldNotify(Object::Ref<InheritedWidget> oldWidget) = 0;
    Object::Ref<Widget> build(Object::Ref<BuildContext> context) override;
    Object::Ref<Element> createElement() override;

protected:
    Object::Ref<Widget> _child;
};

template <typename T, typename std::enable_if<std::is_base_of<InheritedWidget, T>::value>::type *>
inline Object::Ref<T> BuildContext::dependOnInheritedWidgetOfExactType()
{
    auto iter = this->_inheritances.find(typeid(T).name());
    if (iter == this->_inheritances.end())
        return nullptr;
    return iter->second->cast<T>();
}
