#include "async_runtime/elements/element.h"
#include "async_runtime/widgets/notification_listener.h"

// NotificationListener Element
NotificationListenerElement::NotificationListenerElement(Object::Ref<NotificationListener> widget) : _notificationListenerWidget(widget), StatelessElement(widget) {}

void NotificationListenerElement::update(Object::Ref<Widget> newWidget)
{
    this->_notificationListenerWidget = newWidget->cast<NotificationListener>();
    assert(this->_notificationListenerWidget);
    StatelessElement::update(newWidget);
}

void NotificationListenerElement::notify(Object::Ref<Widget> newWidget)
{
    this->_notificationListenerWidget = newWidget->cast<NotificationListener>();
    assert(this->_notificationListenerWidget);
    StatelessElement::notify(newWidget);
}

void NotificationListenerElement::detach()
{
    _notificationListenerWidget = nullptr;
    StatelessElement::detach();
}