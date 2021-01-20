#include "async_runtime/basic/change_notifier.h"

void ChangeNotifier::notifyListeners()
{
    for (auto const &it : this->_listeners)
        (*(it.second))(Object::cast<>(this));
}