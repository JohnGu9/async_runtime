#include "async_runtime/object.h"

Lock::~Lock()
{
    std::unique_lock<std::mutex> lock(this->_mutex);
    assert(this->_sharedCounter == 0 && this->_unique == nullptr);
}

option<Lock::SharedLock> Lock::sharedLock()
{
    return Object::create<Lock::SharedLock>(*this);
}

option<Lock::UniqueLock> Lock::uniqueLock()
{
    return Object::create<Lock::UniqueLock>(*this);
}

Lock::SharedLock::SharedLock(Lock &lock) : _lock(lock)
{
    std::unique_lock<std::mutex> ul(_lock._mutex);
    _lock._condition.wait(ul, [this] { return this->_lock._unique == nullptr; });
    _lock._sharedCounter++;
}

Lock::SharedLock::~SharedLock()
{
    {
        std::unique_lock<std::mutex> ul(_lock._mutex);
        assert(_lock._sharedCounter > 0);
        _lock._sharedCounter--;
    }
    _lock._condition.notify_one();
}

Lock::UniqueLock::UniqueLock(Lock &lock) : _lock(lock)
{
    std::unique_lock<std::mutex> ul(_lock._mutex);
    _lock._condition.wait(ul, [this] { return this->_lock._unique == nullptr; });
    _lock._unique = this; // block read operations that happen after write
    _lock._condition.wait(ul, [this] { return this->_lock._sharedCounter == 0; });
}

Lock::UniqueLock::~UniqueLock()
{
    {
        std::unique_lock<std::mutex> ul(_lock._mutex);
        assert(_lock._sharedCounter == 0);
        _lock._unique = nullptr;
    }
    _lock._condition.notify_all();
}