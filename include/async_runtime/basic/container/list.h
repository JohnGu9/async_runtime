#pragma once

#include <initializer_list>
#include <memory>
#include <deque>

template <typename T>
class List : public std::shared_ptr<std::deque<T>>
{
    explicit List(std::nullptr_t) : std::shared_ptr<std::deque<T>>(std::make_shared<std::deque<T>>()) {}

public:
    using iterator = typename std::deque<T>::iterator;
    using const_iterator = typename std::deque<T>::const_iterator;

    static List<T> empty() { return List<T>(nullptr); }
    explicit List() {}
    List(const List<T> &other) : std::shared_ptr<std::deque<T>>(std::make_shared<std::deque<T>>(*other)) { assert(*this); }
    List(std::initializer_list<T> &&ls)
        : std::shared_ptr<std::deque<T>>(std::make_shared<std::deque<T>>(std::forward<std::initializer_list<T>>(ls))) { assert(*this); }

    List<T> &operator=(std::initializer_list<T> &&ls)
    {
        std::shared_ptr<std::deque<T>>::operator=(
            std::make_shared<std::deque<T>>(std::forward<std::initializer_list<T>>(ls)));
        assert(*this);
        return *this;
    }

    List<T> &operator=(const List<T> &other)
    {
        std::shared_ptr<std::deque<T>>::operator=(static_cast<const std::shared_ptr<std::deque<T>> &>(other));
        return *this;
    }

    List<T> &operator=(std::nullptr_t t)
    {
        std::shared_ptr<std::deque<T>>::operator=(t);
        return *this;
    }

    T &operator[](size_t index) { return (*this)->operator[](index); }
    T &operator[](size_t index) const { return (*this)->operator[](index); }

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

    List<T> copy() const
    {
        return List<T>(*this);
    }
};