#pragma once

#include <memory>
#include "function.h"

template <typename T>
using weakref = std::weak_ptr<T>;

// nullable object
template <typename T>
class option;
// non-nullable object
template <typename T>
class ref;

template <typename T>
class option : public std::shared_ptr<T>
{
public:
    static option<T> null()
    {
        static const option<T> instance = nullptr;
        return instance;
    }
    option() {}
    option(std::nullptr_t) : std::shared_ptr<T>(nullptr) {}

    template <typename R, typename std::enable_if<std::is_base_of<T, R>::value>::type * = nullptr>
    option(const ref<R> &other);
    template <typename R, typename std::enable_if<std::is_base_of<T, R>::value>::type * = nullptr>
    option(const std::shared_ptr<R> &other) : std::shared_ptr<T>(std::static_pointer_cast<T>(other)){};
    template <typename R, typename std::enable_if<std::is_base_of<T, R>::value>::type * = nullptr>
    option(const option<R> &other) : std::shared_ptr<T>(std::static_pointer_cast<T>(other)) {}

    bool isNotNull(ref<T> &) const;
    ref<T> isNotNullElse(Function<ref<T>()>) const;
    ref<T> assertNotNull() const;

    T *operator->() const = delete;
    operator bool() const = delete;
};

template <typename T>
bool operator==(const option<T> &opt, std::nullptr_t) { return static_cast<std::shared_ptr<T>>(opt) == nullptr; }
template <typename T>
bool operator!=(const option<T> &opt, std::nullptr_t) { return static_cast<std::shared_ptr<T>>(opt) != nullptr; }

template <typename T>
class ref : public option<T>
{
    template <typename R>
    friend class option;

    friend class Object;

public:
    ref(std::nullptr_t) = delete;

    template <typename R, typename std::enable_if<std::is_base_of<T, R>::value>::type * = nullptr>
    ref(const ref<R> &other) : option<T>(other) {}

    T *operator->() const { return std::shared_ptr<T>::operator->(); }

    std::shared_ptr<T> &operator=(std::nullptr_t) = delete;
    bool operator==(std::nullptr_t) const = delete;
    bool operator!=(std::nullptr_t) const = delete;

    bool isNotNull(ref<T> &) const = delete;
    ref<T> isNotNullElse(Function<ref<T>()>) const = delete;
    ref<T> assertNotNull() const = delete;

protected:
    ref() {}

    template <typename R, typename std::enable_if<std::is_base_of<T, R>::value>::type * = nullptr>
    ref(const option<R> &other) : option<T>(other) {}

    template <typename R, typename std::enable_if<std::is_base_of<T, R>::value>::type * = nullptr>
    ref(const std::shared_ptr<R> &other) : option<T>(other) {}
};

// non-nullable object
template <typename T>
class lateref : public ref<T>
{
    template <typename R>
    friend class option;

    friend class Object;

public:
    lateref() : ref<T>() {}

    template <typename R, typename std::enable_if<std::is_base_of<T, R>::value>::type * = nullptr>
    lateref(const ref<R> &other) : ref<T>(other) {}

    bool operator==(std::nullptr_t) const { return static_cast<std::shared_ptr<T>>(*this) == nullptr; }
    bool operator!=(std::nullptr_t) const { return static_cast<std::shared_ptr<T>>(*this) != nullptr; }

protected:
    template <typename R, typename std::enable_if<std::is_base_of<T, R>::value>::type * = nullptr>
    lateref(const option<R> &other) : ref<T>(other) {}

    template <typename R, typename std::enable_if<std::is_base_of<T, R>::value>::type * = nullptr>
    lateref(const std::shared_ptr<R> &other) : ref<T>(other) {}
};

template <typename T>
template <typename R, typename std::enable_if<std::is_base_of<T, R>::value>::type *>
option<T>::option(const ref<R> &other) : std::shared_ptr<T>(static_cast<std::shared_ptr<R>>(other)) {}

template <typename T>
bool option<T>::isNotNull(ref<T> &object) const
{
    const std::shared_ptr<T> ptr = static_cast<const std::shared_ptr<T>>(*this);
    if (ptr != nullptr)
    {
        object = ptr;
        return true;
    }
    else
    {
        return false;
    }
}

template <typename T>
ref<T> option<T>::isNotNullElse(Function<ref<T>()> fn) const
{
    const std::shared_ptr<T> ptr = static_cast<const std::shared_ptr<T>>(*this);
    if (ptr != nullptr)
    {
        return ref<T>(ptr);
    }
    else
    {
        return fn();
    }
}

template <typename T>
ref<T> option<T>::assertNotNull() const
{
    const std::shared_ptr<T> ptr = static_cast<const std::shared_ptr<T>>(*this);
    if (ptr != nullptr)
    {
        return ref<T>(ptr);
    }
    else
    {
        throw "Assert not null but the object is null indeed. ";
    }
}

namespace std
{
    template <typename T>
    struct hash<option<T>>
    {

        std::size_t operator()(const option<T> &other) const
        {
            static const auto hs = hash<std::shared_ptr<T>>();
            return hs(static_cast<std::shared_ptr<T>>(other));
        }
    };

    template <typename T>
    struct hash<::ref<T>>
    {
        std::size_t operator()(const ::ref<T> &other) const
        {
            static const auto hs = hash<std::shared_ptr<T>>();
            return hs(static_cast<std::shared_ptr<T>>(other));
        }
    };
} // namespace std