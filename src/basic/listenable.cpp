#include "async_runtime/basic/listenable.h"
#include <assert.h>

Listenable::Listenable() : _listeners(), _isDisposed(false) {}

void Listenable::addListener(Function<void(Object::Ref<Listenable>)> fn)
{
    assert(!this->_isDisposed);
    this->_listeners.insert(fn);
}

void Listenable::removeListener(Function<void(Object::Ref<Listenable>)> fn)
{
    using iterator = Set<Function<void(Object::Ref<Listenable>)>>::iterator;
    const iterator it = this->_listeners.find(fn);
    if (it != this->_listeners.end())
        this->_listeners.erase(it);
}

void Listenable::dispose()
{
    this->_isDisposed = true;
    this->_listeners.clear();
}
