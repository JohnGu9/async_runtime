#include "async_runtime/basic/notification.h"
#include "async_runtime/elements/element.h"
#include "async_runtime/widgets/notification_listener.h"

void Notification::dispatch(Object::Ref<BuildContext> context)
{
    Object::Ref<Element> element = context->cast<Element>();
    Object::Ref<Notification> self = Object::cast<>(this);
    element->visitAncestor([self](Object::Ref<Element> element) -> bool {
        if (Object::Ref<NotificationListenerElement> listenerElement = element->cast<NotificationListenerElement>())
            return listenerElement->_notificationListenerWidget->onNotification(self);
        return false;
    });
}