#pragma once

#include "stateless_widget.h"

class Notification : public virtual Object
{
public:
    virtual void dispatch(ref<BuildContext> context);
};

template <typename T = Notification, typename Object::Base<Notification>::isBaseOf<T>::type * = nullptr>
class NotificationListener;

template <>
class NotificationListener<Notification> : public StatelessWidget
{
public:
    NotificationListener(ref<Widget> child, Function<bool(ref<Notification> notification)> onNotification, option<Key> key = nullptr);
    virtual bool onNotification(ref<Notification> notification) { return this->_onNotification(notification); }

protected:
    ref<Widget> build(ref<BuildContext> context) override;
    ref<Element> createElement() override;
    finalref<Widget> _child;
    Function<bool(ref<Notification> notification)> _onNotification;
};

template <typename T, typename Object::Base<Notification>::isBaseOf<T>::type *>
class NotificationListener : public NotificationListener<Notification>
{
public:
    NotificationListener(ref<Widget> child, Function<bool(ref<T> notification)> onNotification, option<Key> key = nullptr)
        : NotificationListener<Notification>(
              child, [onNotification](ref<Notification> notification) -> bool
              {
                  lateref<T> n;
                  if (notification->cast<T>().isNotNull(n))
                      return onNotification(n);
                  return false; },
              key)
    {
    }
};
