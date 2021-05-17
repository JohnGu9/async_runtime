#include "async_runtime/elements/notification_listener_element.h"
#include "async_runtime/widgets/notification_listener.h"

// NotificationListener Element
NotificationListenerElement::NotificationListenerElement(ref<NotificationListener<Notification>> widget)
    : StatelessElement(widget), _notificationListenerWidget(widget) {}

void NotificationListenerElement::update(ref<Widget> newWidget)
{
    this->_notificationListenerWidget = newWidget->covariant<NotificationListener<Notification>>();
    StatelessElement::update(newWidget);
}

void NotificationListenerElement::notify(ref<Widget> newWidget)
{
    this->_notificationListenerWidget = newWidget->covariant<NotificationListener<Notification>>();
    StatelessElement::notify(newWidget);
}

void NotificationListenerElement::detach()
{
    StatelessElement::detach();
}