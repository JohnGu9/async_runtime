#include "async_runtime/basic/change_notifier.h"

void ChangeNotifier::notifyListeners()
{
    for (auto const &fn : this->_listeners)
        fn(Object::cast<>(this));
}