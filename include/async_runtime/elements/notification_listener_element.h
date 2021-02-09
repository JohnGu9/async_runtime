#pragma once

#include "stateless_element.h"

class NotificationListener;
class NotificationListenerElement : public StatelessElement
{
public:
    NotificationListenerElement(Object::Ref<NotificationListener> child);
    Object::Ref<NotificationListener> _notificationListenerWidget;

    void detach() override;
    void update(Object::Ref<Widget> newWidget) override;
    void notify(Object::Ref<Widget> newWidget) override;
};