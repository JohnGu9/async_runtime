#pragma once

// Do not include this header directly
// Please try to include object.h rather than this

#include <condition_variable>
#include "../object.h"

// C++11 does not include read/write lock
// read/write lock only available after C++14
// So implement C++11 compatible version here

class Lock : public Object
{
public:
    class SharedLock;
    class UniqueLock;
    class InvailedLock;
    class WithLockMixin;

    Lock() {}
    ~Lock();

    virtual ref<Lock::SharedLock> sharedLock();
    virtual ref<Lock::UniqueLock> uniqueLock();

protected:
    size_t _sharedCounter = 0;
    Lock::UniqueLock *_unique = nullptr;

    std::mutex _mutex;
    std::condition_variable _condition;
};

class Lock::SharedLock : public Object
{
public:
    SharedLock(Lock &lock);
    ~SharedLock();

protected:
    Lock &_lock;
};

class Lock::UniqueLock : public Object
{
public:
    UniqueLock(Lock &lock);
    ~UniqueLock();

protected:
    Lock &_lock;
};

class Lock::InvailedLock : public Lock
{
    ref<Lock::SharedLock> sharedLock() override { return nullptr; }
    ref<Lock::UniqueLock> uniqueLock() override { return nullptr; }
};

class Lock::WithLockMixin
{
    ref<Lock> _lock;

public:
    WithLockMixin(ref<Lock> lock = nullptr) : _lock(lock == nullptr ? Object::create<Lock>() : lock) {}
    WithLockMixin(const WithLockMixin &other) : _lock(other.lock) { assert(this->_lock != nullptr); }

    const ref<Lock> &lock = _lock;

    WithLockMixin &operator=(const WithLockMixin &other)
    {
        this->_lock = other.lock;
        assert(this->_lock != nullptr);
        return *this;
    }
};