#pragma once

#include "../basic/notification.h"
#include "stateless_widget.h"

class NotificationListener : public StatelessWidget
{
public:
    NotificationListener(ref<Widget> child, Function<bool(ref<Notification> notification)> onNotification, ref<Key> key = nullptr);
    virtual bool onNotification(ref<Notification> notification) { return this->_onNotification(notification); }
    ref<Widget> build(ref<BuildContext> context) override;
    ref<Element> createElement() override;

protected:
    ref<Widget> _child;
    Function<bool(ref<Notification> notification)> _onNotification;
};