#include "async_runtime/basic/listenable.h"
#include <assert.h>

Listenable::Listenable() : _listeners(),
                           _isDisposed(false)
{
}

void Listenable::addListener(Function<void(Object::Ref<Listenable>)> fn)
{
    assert(!this->_isDisposed);
    this->_listeners[(size_t)fn._fn.get()] = fn;
}

void Listenable::removeListener(Function<void(Object::Ref<Listenable>)> fn)
{
    using iterator = Object::Map<size_t, Function<void(Object::Ref<Listenable>)>>::iterator;
    const iterator it = this->_listeners.find((size_t)fn._fn.get());
    if (it != this->_listeners.end())
        this->_listeners.erase(it);
}

void Listenable::dispose()
{
    this->_isDisposed = true;
    this->_listeners.clear();
}
