#include "async_runtime/basic/listenable.h"
#include <assert.h>

Listenable::Listenable() : _listeners(Object::create<Set<Function<void(ref<Listenable>)>>>()), _isDisposed(false) {}

void Listenable::addListener(Function<void(ref<Listenable>)> fn)
{
    assert(!this->_isDisposed);
    this->_listeners->insert(fn);
}

void Listenable::removeListener(Function<void(ref<Listenable>)> fn)
{
    using iterator = Set<Function<void(ref<Listenable>)>>::iterator;
    const iterator it = this->_listeners->find(fn);
    if (it != this->_listeners.end())
        this->_listeners->erase(it);
}

void Listenable::dispose()
{
    this->_isDisposed = true;
    Object::detach(this->_listeners);
}
