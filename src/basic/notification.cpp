#include "async_runtime/basic/notification.h"
#include "async_runtime/elements/notification_listener_element.h"
#include "async_runtime/widgets/notification_listener.h"

void Notification::dispatch(ref<BuildContext> context)
{
    ref<Element> element = context->cast<Element>().assertNotNull();
    ref<Notification> self = Object::cast<>(this);
    element->visitAncestor([self](ref<Element> element) -> bool {
        lateref<NotificationListenerElement> listenerElement;
        if (element->cast<NotificationListenerElement>().isNotNull(listenerElement))
        {
            return listenerElement->_notificationListenerWidget->onNotification(self);
        }
        return false;
    });
}