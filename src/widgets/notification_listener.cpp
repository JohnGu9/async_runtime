#include "async_runtime/elements/notification_listener_element.h"
#include "async_runtime/widgets/notification_listener.h"

NotificationListener::NotificationListener(
    Object::Ref<Widget> child,
    Function<bool(Object::Ref<Notification> notification)> onNotification,
    Object::Ref<Key> key)
    : _child(child), _onNotification(onNotification), StatelessWidget(key) {}

Object::Ref<Widget> NotificationListener::build(Object::Ref<BuildContext> context)
{
    return this->_child;
}

Object::Ref<Element> NotificationListener::createElement()
{
    return Object::create<NotificationListenerElement>(Object::cast<>(this));
}
