/**
 * @brief
 * Provide a read-write lock that fully compatable with C++11
 * (only after C++14 that std provide read/write lock)
 * by default, do not include the file.
 * the file with come with container.h
 *
 */

#pragma once
#include "object.h"
#include <condition_variable>

/**
 * @brief
 * Lock provide read-write lock feature
 *
 * @example
 * auto lock = Object::create<Lock>();
 * {
 *      auto readLock = lock->sharedLock();
 *      // do some jobs
 * }
 *
 * {
 *      auto writeLock = lock->uniqueLock();
 *      // do some jobs
 * }
 *
 */
class Lock : public virtual Object
{
public:
    class SharedLock;
    class UniqueLock;
    class InvalidLock;
    class WithLockMixin;

    Lock() {}
    ~Lock();

    virtual option<Lock::SharedLock> sharedLock();
    virtual option<Lock::UniqueLock> uniqueLock();

protected:
    size_t _sharedCounter = 0;
    Lock::UniqueLock *_unique = nullptr;

    std::mutex _mutex;
    std::condition_variable _condition;
};

class Lock::SharedLock : public virtual Object
{
public:
    SharedLock(Lock &lock);
    ~SharedLock();

protected:
    Lock &_lock;
};

class Lock::UniqueLock : public virtual Object
{
public:
    UniqueLock(Lock &lock);
    ~UniqueLock();

protected:
    Lock &_lock;
};

class Lock::InvalidLock : public Lock
{
    option<Lock::SharedLock> sharedLock() override { return nullptr; }
    option<Lock::UniqueLock> uniqueLock() override { return nullptr; }
};

class Lock::WithLockMixin
{
    ref<Lock> _lock;

public:
    WithLockMixin(option<Lock> lock = nullptr) : _lock(lock.isNotNullElse(Object::create<Lock>)) {}
    WithLockMixin(const WithLockMixin &other) : _lock(other.lock) {}

    const ref<Lock> &lock = _lock;

    WithLockMixin &operator=(const WithLockMixin &other)
    {
        this->_lock = other.lock;
        return *this;
    }
};
