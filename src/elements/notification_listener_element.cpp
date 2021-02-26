#include "async_runtime/elements/notification_listener_element.h"
#include "async_runtime/widgets/notification_listener.h"

// NotificationListener Element
NotificationListenerElement::NotificationListenerElement(ref<NotificationListener> widget)
    : _notificationListenerWidget(widget), StatelessElement(widget) {}

void NotificationListenerElement::update(ref<Widget> newWidget)
{
    this->_notificationListenerWidget = newWidget->cast<NotificationListener>();
    assert(this->_notificationListenerWidget);
    StatelessElement::update(newWidget);
}

void NotificationListenerElement::notify(ref<Widget> newWidget)
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