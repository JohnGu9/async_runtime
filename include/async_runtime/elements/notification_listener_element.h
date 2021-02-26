#pragma once

#include "stateless_element.h"

class NotificationListener;
class NotificationListenerElement : public StatelessElement
{
public:
    NotificationListenerElement(ref<NotificationListener> child);
    ref<NotificationListener> _notificationListenerWidget;

    void detach() override;
    void update(ref<Widget> newWidget) override;
    void notify(ref<Widget> newWidget) override;
};