#pragma once
#include "stateless_element.h"
#include "../widgets/notification_listener.h"

class NotificationListenerElement : public StatelessElement
{
public:
    NotificationListenerElement(ref<NotificationListener<Notification>> child);
    ref<NotificationListener<Notification>> _notificationListenerWidget;

    void detach() override;
    void update(ref<Widget> newWidget) override;
    void notify(ref<Widget> newWidget) override;
};
