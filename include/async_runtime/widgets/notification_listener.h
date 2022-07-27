#pragma once
#include "stateless_widget.h"

class Notification : public virtual Object
{
public:
    virtual void dispatch(ref<BuildContext> context);
};

template <typename T = Notification, typename std::enable_if<std::is_base_of<Notification, T>::value>::type * = nullptr>
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

#ifdef _WIN32
#pragma warning(push)
#pragma warning(disable : 4573)
#endif
template <typename T, typename std::enable_if<std::is_base_of<Notification, T>::value>::type *>
class NotificationListener : public NotificationListener<Notification>
{
public:
    NotificationListener(ref<Widget> child, Function<bool(ref<T> notification)> onNotification, option<Key> key = nullptr)
        : NotificationListener<Notification>(
              child, [onNotification](ref<Notification> notification) -> bool { //
                  option<T> n = notification->cast<T>();
                  if_not_null(n) return onNotification(n);
                  end_if() return false;
              },
              key)
    {
    }
};
#ifdef _WIN32
#pragma warning(pop)
#endif
