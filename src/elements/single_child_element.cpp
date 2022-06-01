#include "async_runtime/elements/single_child_element.h"
#include "async_runtime/widgets/stateless_widget.h"
#include "async_runtime/widgets/widget.h"

class SingleChildElement::InvalidWidget : public StatelessWidget
{
public:
    ref<Widget> build(ref<BuildContext>) final
    {
        DEBUG_ASSERT(false);
        throw std::runtime_error("AsyncRuntime Internal Error");
    }
    ref<Element> createElement() final { return StatelessWidget::createElement(); }
};

SingleChildElement::SingleChildElement(ref<Widget> widget) : Element(widget) {}

void SingleChildElement::attachElement(ref<Element> element)
{
    this->_childElement = element;
    this->_childElement->parent = self();
    this->_childElement->attach();
}

void SingleChildElement::reattachElement(ref<Element> element)
{
    this->_childElement->detach();
    this->_childElement = element;
    this->_childElement->parent = self();
    this->_childElement->attach();
}

void SingleChildElement::detachElement()
{
    this->_childElement->detach();

    static finalref<Element> _invalidElement = Object::create<SingleChildElement::InvalidWidget>()->createElement();
    this->_childElement = _invalidElement;
}
