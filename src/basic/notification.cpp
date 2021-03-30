#include "async_runtime/basic/notification.h"
#include "async_runtime/elements/notification_listener_element.h"
#include "async_runtime/widgets/notification_listener.h"

void Notification::dispatch(ref<BuildContext> context)
{
    ref<Element> element = context->covariant<Element>();
    ref<Notification> self = self();
    element->visitAncestor([self](ref<Element> element) -> bool {
        lateref<NotificationListenerElement> listenerElement;
        if (element->cast<NotificationListenerElement>().isNotNull(listenerElement))
        {
            return listenerElement->_notificationListenerWidget->onNotification(self);
        }
        return false;
    });
}