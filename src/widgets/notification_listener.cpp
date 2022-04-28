#include "async_runtime/widgets/notification_listener.h"
#include "async_runtime/elements/notification_listener_element.h"

void Notification::dispatch(ref<BuildContext> context)
{
    ref<Element> element = context->covariant<Element>();
    ref<Notification> self = self();
    element->visitAncestor([self](ref<Element> element) -> bool
                           {
        lateref<NotificationListenerElement> listenerElement;
        if (element->cast<NotificationListenerElement>().isNotNull(listenerElement))
        {
            return listenerElement->_notificationListenerWidget->onNotification(self);
        }
        return false; });
}

NotificationListener<Notification>::NotificationListener(
    ref<Widget> child,
    Function<bool(ref<Notification> notification)> onNotification,
    option<Key> key)
    : StatelessWidget(key), _child(child), _onNotification(onNotification) {}

ref<Widget> NotificationListener<Notification>::build(ref<BuildContext> context)
{
    return this->_child;
}

ref<Element> NotificationListener<Notification>::createElement()
{
    return Object::create<NotificationListenerElement>(self());
}
