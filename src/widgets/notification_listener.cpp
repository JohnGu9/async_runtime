#include "async_runtime/widgets/notification_listener.h"
#include "async_runtime/elements/notification_listener_element.h"

#ifdef _WIN32
#pragma warning(push)
#pragma warning(disable : 4573)
#endif
void Notification::dispatch(ref<BuildContext> context)
{
    ref<Element> element = context->covariant<Element>();
    ref<Notification> self = self();
    element->visitAncestor([self](ref<Element> element) -> bool { //
        option<NotificationListenerElement> listenerElement = element->cast<NotificationListenerElement>();
        if_not_null(listenerElement) return listenerElement->_notificationListenerWidget->onNotification(self);
        else_end() return false;
        end_if()
    });
}
#ifdef _WIN32
#pragma warning(pop)
#endif

NotificationListener<Notification>::NotificationListener(
    ref<Widget> child,
    Function<bool(ref<Notification> notification)> onNotification,
    option<Key> key)
    : StatelessWidget(key), _child(child), _onNotification(onNotification)
{
}

ref<Widget> NotificationListener<Notification>::build(ref<BuildContext> context)
{
    return this->_child;
}

ref<Element> NotificationListener<Notification>::createElement()
{
    return Object::create<NotificationListenerElement>(self());
}
