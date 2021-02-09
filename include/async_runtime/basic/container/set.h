#pragma once

#include <initializer_list>
#include <memory>
#include <unordered_set>

template <typename T>
class Set : public std::shared_ptr<std::unordered_set<T>>, public Lock::WithLockMixin
{
    explicit Set(std::nullptr_t) : std::shared_ptr<std::unordered_set<T>>(std::make_shared<std::unordered_set<T>>()) {}

public:
    using iterator = typename std::unordered_set<T>::iterator;
    using const_iterator = typename std::unordered_set<T>::const_iterator;

    static Set<T> empty() { return Set<T>(nullptr); }
    explicit Set() {}
    Set(const Set<T> &other)
        : std::shared_ptr<std::unordered_set<T>>(std::make_shared<std::unordered_set<T>>(*other)), Lock::WithLockMixin(other) { assert(*this); }
    Set(std::initializer_list<T> &&ls)
        : std::shared_ptr<std::unordered_set<T>>(std::make_shared<std::unordered_set<T>>(std::forward<std::initializer_list<T>>(ls))) { assert(*this); }

    Set<T> &operator=(std::initializer_list<T> &&ls)
    {
        std::shared_ptr<std::unordered_set<T>>::operator=(
            std::make_shared<std::unordered_set<T>>(std::forward<std::initializer_list<T>>(ls)));
        assert(*this);
        return *this;
    }

    Set<T> &operator=(const Set<T> &other)
    {
        std::shared_ptr<std::unordered_set<T>>::operator=(static_cast<const std::shared_ptr<std::unordered_set<T>> &>(other));
        Lock::WithLockMixin::operator=(other);
        return *this;
    }

    Set<T> &operator=(std::nullptr_t t)
    {
        std::shared_ptr<std::unordered_set<T>>::operator=(t);
        return *this;
    }

    iterator begin()
    {
        assert(*this);
        return (*this)->begin();
    }
    const_iterator begin() const
    {
        assert(*this);
        return (*this)->begin();
    }

    iterator end()
    {
        assert(*this);
        return (*this)->end();
    }
    const_iterator end() const
    {
        assert(*this);
        return (*this)->end();
    }

    Set<T> copy() const
    {
        return Set<T>(*this);
    }

    bool any(Function<bool(const T &)> fn)
    {
        for (auto &iter : **this)
            if (fn(iter))
                return true;
        return false;
    }

    bool every(Function<bool(const T &)> fn)
    {
        for (auto &iter : **this)
            if (not fn(iter))
                return false;
        return true;
    }
};