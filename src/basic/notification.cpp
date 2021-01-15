#include "framework/basic/notification.h"
#include "framework/elements/element.h"
#include "framework/widgets/widget.h"

void Notification::dispatch(Object::Ref<BuildContext> context)
{
    Object::Ref<Element> element = context->cast<Element>();
    Object::Ref<Notification> self = Object::self(this);
    element->visitAncestor([self](Object::Ref<Element> element) -> bool {
        if (Object::Ref<NotificationListenerElement> listenerElement = element->cast<NotificationListenerElement>())
            return listenerElement->_notificationListenerWidget->onNotification(self);
        return false;
    });
}