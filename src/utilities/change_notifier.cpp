#include "async_runtime/utilities/change_notifier.h"

void ChangeNotifier::notifyListeners()
{
    for (auto const &fn : this->_listeners)
        fn();
}
