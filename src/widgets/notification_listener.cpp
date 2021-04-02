#include "async_runtime/elements/notification_listener_element.h"
#include "async_runtime/widgets/notification_listener.h"

NotificationListener::NotificationListener(
    ref<Widget> child,
    Function<bool(ref<Notification> notification)> onNotification,
    option<Key> key)
    : StatelessWidget(key), _child(child), _onNotification(onNotification) {}

ref<Widget> NotificationListener::build(ref<BuildContext> context)
{
    return this->_child;
}

ref<Element> NotificationListener::createElement()
{
    return Object::create<NotificationListenerElement>(self());
}
