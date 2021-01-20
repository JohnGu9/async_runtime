#pragma once

#include "../basic/notification.h"
#include "stateless_widget.h"

class NotificationListener : public StatelessWidget
{
public:
    NotificationListener(Object::Ref<Widget> child, Fn<bool(Object::Ref<Notification> notification)> onNotification, Object::Ref<Key> key = nullptr);
    virtual bool onNotification(Object::Ref<Notification> notification) { return this->_onNotification(notification); }
    Object::Ref<Widget> build(Object::Ref<BuildContext> context) override;
    Object::Ref<Element> createElement() override;

protected:
    Object::Ref<Widget> _child;
    Fn<bool(Object::Ref<Notification> notification)> _onNotification;
};