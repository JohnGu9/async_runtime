#pragma once

#include <memory>
#include <functional>
#include <exception>
#define finalref const ref

// nullable object
template <typename T>
class option;
// non-nullable object
template <typename T>
class ref;
// non-nullable object
template <typename T>
class lateref;
// weakref for nullable object
template <typename T>
class weakref;

// the namespace not for public usage
// ar just for async runtime internal usage
namespace ar
{
    template <typename T>
    class RefImplement;

    template <typename T>
    class ToRefMixin
    {
    public:
        virtual ~ToRefMixin() {}
        virtual bool isNotNull(ref<T> &) const = 0;
        virtual ref<T> isNotNullElse(std::function<ref<T>()>) const = 0;
        virtual ref<T> assertNotNull() const = 0;
    };
};

template <typename T>
class option : public std::shared_ptr<T>, public ar::ToRefMixin<T>
{
    template <typename R>
    friend class weakref;

    template <typename R>
    friend class ar::RefImplement;

    template <typename R>
    friend class ref;

    template <typename R>
    friend class lateref;

    friend class Object;

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
    option(const option<R> &other) : std::shared_ptr<T>(std::static_pointer_cast<T>(other)) {}

    bool isNotNull(ref<T> &) const override;
    ref<T> isNotNullElse(std::function<ref<T>()>) const override;
    ref<T> assertNotNull() const override;

    T *operator->() const = delete;
    operator bool() const = delete;

protected:
    template <typename R, typename std::enable_if<std::is_base_of<T, R>::value>::type * = nullptr>
    option(const std::shared_ptr<R> &other) : std::shared_ptr<T>(std::static_pointer_cast<T>(other)){};
};

template <typename T>
class ar::RefImplement : public option<T>
{
    template <typename R>
    friend class option;

    template <typename R>
    friend class weakref;

    friend class Object;

public:
    RefImplement(std::nullptr_t) = delete;

    template <typename R, typename std::enable_if<std::is_base_of<T, R>::value>::type * = nullptr>
    RefImplement(const RefImplement<R> &other) : option<T>(other) {}

    T *operator->() const
    {
        assert(std::shared_ptr<T>::get() && "NullReference Error! By default this error cause by lateref that use before assgin a non-null reference. ");
        return std::shared_ptr<T>::operator->();
    }

    std::shared_ptr<T> &operator=(std::nullptr_t) = delete;
    bool operator==(std::nullptr_t) const = delete;
    bool operator!=(std::nullptr_t) const = delete;

protected:
    RefImplement() {}

    template <typename R, typename std::enable_if<std::is_base_of<T, R>::value>::type * = nullptr>
    RefImplement(const option<R> &other) : option<T>(other) {}

    template <typename R, typename std::enable_if<std::is_base_of<T, R>::value>::type * = nullptr>
    RefImplement(const std::shared_ptr<R> &other) : option<T>(other) {}

    bool isNotNull(ref<T> &object) const override { return option<T>::isNotNull(object); }
    ref<T> isNotNullElse(std::function<ref<T>()> fn) const override { return option<T>::isNotNullElse(fn); }
    ref<T> assertNotNull() const override { return option<T>::assertNotNull(); }
};

template <typename T>
class ref : public ar::RefImplement<T>
{
    template <typename R>
    friend class option;

    template <typename R>
    friend class weakref;

    friend class Object;

public:
    template <typename R, typename std::enable_if<std::is_base_of<T, R>::value>::type * = nullptr>
    ref(const ref<R> &other) : ar::RefImplement<T>(other) {}

protected:
    ref() {}

    template <typename R, typename std::enable_if<std::is_base_of<T, R>::value>::type * = nullptr>
    ref(const option<R> &other) : ar::RefImplement<T>(other) {}

    template <typename R, typename std::enable_if<std::is_base_of<T, R>::value>::type * = nullptr>
    ref(const std::shared_ptr<R> &other) : ar::RefImplement<T>(other) {}
};

// non-nullable object
template <typename T>
class lateref : public ref<T>
{
    template <typename R>
    friend class option;

    template <typename R>
    friend class weakref;

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
class weakref : public std::weak_ptr<T>, public ar::ToRefMixin<T>
{
public:
    weakref() {}
    weakref(std::nullptr_t) {}
    template <typename R, typename std::enable_if<std::is_base_of<T, R>::value>::type * = nullptr>
    weakref(const option<R> &other) : std::weak_ptr<T>(other) {}

    std::weak_ptr<T> lock() const = delete;

    ref<T> assertNotNull() const override
    {
        const std::shared_ptr<T> ptr = std::weak_ptr<T>::lock();
        if (ptr != nullptr)
        {
            return ref<T>(ptr);
        }
        else
        {
            throw std::runtime_error(std::string(typeid(*this).name()) + " assert not null on a null ref. ");
        }
    }

    ref<T> isNotNullElse(std::function<ref<T>()> fn) const override
    {
        const std::shared_ptr<T> ptr = std::weak_ptr<T>::lock();
        if (ptr != nullptr)
        {
            return ref<T>(ptr);
        }
        else
        {
            return fn();
        }
    }

    bool isNotNull(ref<T> &object) const override
    {
        const std::shared_ptr<T> ptr = std::weak_ptr<T>::lock();
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

    option<T> toOption() const
    {
        return option<T>(std::weak_ptr<T>::lock());
    }
};

/// function implement

template <typename T>
bool operator==(const option<T> &opt, std::nullptr_t) { return static_cast<std::shared_ptr<T>>(opt) == nullptr; }
template <typename T>
bool operator!=(const option<T> &opt, std::nullptr_t) { return static_cast<std::shared_ptr<T>>(opt) != nullptr; }

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
ref<T> option<T>::isNotNullElse(std::function<ref<T>()> fn) const
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
        throw std::runtime_error(std::string(typeid(*this).name()) + " assert not null on a null ref. ");
    }
}

namespace std
{
    template <typename T>
    struct hash<::option<T>>
    {
        std::size_t operator()(const ::option<T> &other) const
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
