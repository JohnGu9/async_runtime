#pragma once

#include "stateless_widget.h"

class Notification : public virtual Object
{
public:
    virtual void dispatch(ref<BuildContext> context);
};

class NotificationListener : public StatelessWidget
{
public:
    NotificationListener(ref<Widget> child, Function<bool(ref<Notification> notification)> onNotification, option<Key> key = nullptr);
    virtual bool onNotification(ref<Notification> notification) { return this->_onNotification(notification); }
    ref<Widget> build(ref<BuildContext> context) override;
    ref<Element> createElement() override;

protected:
    finalref<Widget> _child;
    Function<bool(ref<Notification> notification)> _onNotification;
};