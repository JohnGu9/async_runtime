#include "async_runtime/utilities/listenable.h"

Listenable::Listenable() : _listeners(Object::create<Set<Function<void()>>>()), _isDisposed(false) {}

void Listenable::addListener(Function<void()> fn)
{
    RUNTIME_ASSERT(!this->_isDisposed, "addListener on a disposed Listenable");
    this->_listeners->insert(fn);
}

void Listenable::removeListener(Function<void()> fn)
{
    this->_listeners->remove(fn);
}

void Listenable::dispose()
{
    this->_isDisposed = true;
    this->_listeners->clear();
}
