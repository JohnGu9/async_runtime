/**
 * @brief 
 * The mechanism of nullsafety system
 * Implement base on std::shared_ptr and std::weak_ptr (don't be confused by std::ref)
 * Thread safe and memory safe
 * 
 * 
 * inherits layout:
 * 
 * std::shared_ptr
 *       ^
 *       |
 *  _async_runtime::OptionImplement
 *       ^
 *       |
 *  option
 * 
 * 
 * std::shared_ptr
 *       ^
 *       |
 *  _async_runtime::RefImplent
 *       ^
 *       |
 *  ref
 *       ^
 *       |
 *  lateref
 * 
 * 
 *  std::weak_ptr
 *       ^
 *       |
 *  weakref
 * 
 * 
 * @example
 * ref<Object> object = Object::create<Object>(); // create a object through Object::create
 * ref<Object> refOfObject = object; // another ref of object
 * 
 * option<Object> opt = object; // nullable ref and lock the object resource
 * opt = nullptr; // release ref and unlock the resource
 * 
 * weakref<Object> wr = object; // weak ref that point toward a ref that doesn't lock the resource
 * 
 * refOfObject->function(); // only ref call directly call member. option and weakref can not.
 * // only way option and weakref call member is to change option and weakref to ref
 * 
 * 
 * // change option(/weakref) to ref through ToRefMixin api
 * object = opt.assertNotNull(); // assert is not a good option change that may cause error
 * 
 * if (opt.isNotNull(object)) { // null check is a good option
 *     // object is not null here 
 * }
 * 
 * object = opt.isNotNullElse([]() ->ref<Object> { return Object::create<Object>(); }) // if opt is not null change opt to ref, otherwise create a new Object
 * object = opt.isNotNullElse(Object::create<Object>); another syntax
 * 
 */

#pragma once

#include <memory>
#include <functional>
#include <exception>
#include <assert.h>

#define finalref const ref
#define finaloption const option
#define _ASYNC_RUNTIME_FRIEND_FAMILY                \
    template <typename R>                           \
    friend class ::_async_runtime::OptionImplement; \
    template <typename R>                           \
    friend class ::option;                          \
    template <typename R>                           \
    friend class ::_async_runtime::RefImplement;    \
    template <typename R>                           \
    friend class ::ref;                             \
    template <typename R>                           \
    friend class ::lateref;                         \
    template <typename R>                           \
    friend class ::weakref;                         \
    friend class Object;                            \
    template <typename R>                           \
    friend class Future;                            \
    template <typename R>                           \
    friend class Stream;

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
// _async_runtime just for async runtime internal usage
namespace _async_runtime
{
    template <typename T>
    class OptionImplement;

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
class _async_runtime::OptionImplement : protected std::shared_ptr<T>, public _async_runtime::ToRefMixin<T>
{
    _ASYNC_RUNTIME_FRIEND_FAMILY;

public:
    bool isNotNull(ref<T> &) const override;
    ref<T> isNotNullElse(std::function<ref<T>()>) const override;
    ref<T> assertNotNull() const override;
    size_t hashCode() const
    {
        static const auto hs = std::hash<std::shared_ptr<T>>();
        return hs(static_cast<const std::shared_ptr<T> &>(*this));
    }

    T *get() const { return std::shared_ptr<T>::get(); }
    T *operator->() const = delete;
    T &operator*() const = delete;
    operator bool() const = delete;

protected:
    OptionImplement() {}
    OptionImplement(std::nullptr_t) : std::shared_ptr<T>(nullptr) {}
    template <typename R, typename std::enable_if<std::is_base_of<T, R>::value>::type * = nullptr>
    OptionImplement(const std::shared_ptr<R> &other) : std::shared_ptr<T>(other){};
};

template <typename T>
class option : public _async_runtime::OptionImplement<T>
{
    _ASYNC_RUNTIME_FRIEND_FAMILY;

    template <typename R>
    friend bool operator==(const option<R> &opt, std::nullptr_t);
    template <typename R>
    friend bool operator!=(const option<R> &opt, std::nullptr_t);

    template <typename X, typename Y>
    friend bool operator==(const option<X> &object0, const option<Y> &object1);
    template <typename X, typename Y>
    friend bool operator!=(const option<X> &object0, const option<Y> &object1);

public:
    option() {}
    option(std::nullptr_t) : _async_runtime::OptionImplement<T>(nullptr) {}
    template <typename R, typename std::enable_if<std::is_base_of<T, R>::value>::type * = nullptr>
    option(const ref<R> &other);
    template <typename R, typename std::enable_if<std::is_base_of<T, R>::value>::type * = nullptr>
    option(const option<R> &other) : _async_runtime::OptionImplement<T>(other) {}
    template <typename R, typename std::enable_if<std::is_base_of<T, R>::value>::type * = nullptr>
    option(const std::shared_ptr<R> &other) : _async_runtime::OptionImplement<T>(other){};

    /**
     * @brief Construct a new option object
     * ref quick init also available for option (just forward argument to ref)
     * so that option no need to specialize template
     * but for nullsafety, option quick init need at least one argument (zero arguments will cause null ref init)
     * this api should only used inside nullsafety system, not for public usage
     *  
     * @tparam First 
     * @tparam Args 
     * @param first 
     * @param args 
     */
    template <typename First, typename... Args>
    option(const First &first, Args &&...args);
};

template <typename T>
class _async_runtime::RefImplement : protected std::shared_ptr<T>
{
    _ASYNC_RUNTIME_FRIEND_FAMILY;

public:
    size_t hashCode() const
    {
        static const auto hs = std::hash<std::shared_ptr<T>>();
        return hs(static_cast<const std::shared_ptr<T> &>(*this));
    }

    T &operator*() const
    {
        assert(std::shared_ptr<T>::get() && "lateref Uninitiated NullReference Error! By default this error cause by lateref that use before assgin a non-null reference. ");
        return std::shared_ptr<T>::operator*();
    }

    T *operator->() const
    {
        assert(std::shared_ptr<T>::get() && "lateref Uninitiated NullReference Error! By default this error cause by lateref that use before assgin a non-null reference. ");
        return std::shared_ptr<T>::operator->();
    }

    T *get() const
    {
        assert(std::shared_ptr<T>::get() && "lateref Uninitiated NullReference Error! By default this error cause by lateref that use before assgin a non-null reference. ");
        return std::shared_ptr<T>::get();
    }

    std::shared_ptr<T> &operator=(std::nullptr_t) = delete;
    bool operator==(std::nullptr_t) const = delete;
    bool operator!=(std::nullptr_t) const = delete;
    operator bool() const = delete;

protected:
    RefImplement() {}
    RefImplement(std::nullptr_t) = delete;

    template <typename R, typename std::enable_if<std::is_base_of<T, R>::value>::type * = nullptr>
    RefImplement(const std::shared_ptr<R> &other) : std::shared_ptr<T>(other) {}
};

template <typename T>
class ref : public _async_runtime::RefImplement<T>
{
    _ASYNC_RUNTIME_FRIEND_FAMILY;
    template <typename X, typename Y>
    friend bool operator==(const ref<X> &object0, const ref<Y> &object1);

public:
    template <typename R, typename std::enable_if<std::is_base_of<T, R>::value>::type * = nullptr>
    ref(const ref<R> &other) : _async_runtime::RefImplement<T>(other) {}

protected:
    ref() {}

    template <typename R, typename std::enable_if<std::is_base_of<T, R>::value>::type * = nullptr>
    ref(const std::shared_ptr<R> &other) : _async_runtime::RefImplement<T>(other) {}
};

// non-nullable object
template <typename T>
class lateref : public ref<T>
{
    _ASYNC_RUNTIME_FRIEND_FAMILY;

public:
    explicit lateref() : ref<T>() {}

    template <typename R, typename std::enable_if<std::is_base_of<T, R>::value>::type * = nullptr>
    lateref(const ref<R> &other) : ref<T>(other) {}
};

template <typename T>
class weakref : public std::weak_ptr<T>, public _async_runtime::ToRefMixin<T>
{
public:
    weakref() {}
    weakref(std::nullptr_t) {}
    template <typename R, typename std::enable_if<std::is_base_of<T, R>::value>::type * = nullptr>
    weakref(const option<R> &other) : std::weak_ptr<T>(other) {}
    template <typename R, typename std::enable_if<std::is_base_of<T, R>::value>::type * = nullptr>
    weakref(const ref<R> &other) : std::weak_ptr<T>(other) {}

    std::weak_ptr<T> lock() const = delete;

    ref<T> assertNotNull() const override
    {
        const std::shared_ptr<T> ptr = std::weak_ptr<T>::lock();
        if (ptr != nullptr)
            return ref<T>(ptr);
        else
            throw std::runtime_error(std::string(typeid(*this).name()) + " assert not null on a null ref. ");
    }

    ref<T> isNotNullElse(std::function<ref<T>()> fn) const override
    {
        const std::shared_ptr<T> ptr = std::weak_ptr<T>::lock();
        if (ptr != nullptr)
            return ref<T>(ptr);
        else
            return fn();
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
            return false;
    }

    option<T> toOption() const
    {
        return option<T>(std::weak_ptr<T>::lock());
    }
};

template <typename T>
bool operator==(const option<T> &opt, std::nullptr_t) { return static_cast<const std::shared_ptr<T> &>(opt) == nullptr; }
template <typename T>
bool operator!=(const option<T> &opt, std::nullptr_t) { return static_cast<const std::shared_ptr<T> &>(opt) != nullptr; }

template <typename T, typename R>
bool operator==(const option<T> &object0, const option<R> &object1) { return static_cast<const std::shared_ptr<T> &>(object0) == static_cast<const std::shared_ptr<R> &>(object1); }
template <typename T, typename R>
bool operator!=(const option<T> &object0, const option<R> &object1) { return !(object0 == object1); }

template <typename T, typename R>
bool operator==(const ref<T> &object0, const ref<R> &object1) { return static_cast<const std::shared_ptr<T> &>(object0) == static_cast<const std::shared_ptr<R> &>(object1); }
template <typename T, typename R>
bool operator!=(const ref<T> &object0, const ref<R> &object1) { return !(object0 == object1); }

template <typename T>
template <typename R, typename std::enable_if<std::is_base_of<T, R>::value>::type *>
option<T>::option(const ref<R> &other)
    : _async_runtime::OptionImplement<T>(other) {}

template <typename T>
template <typename First, typename... Args>
option<T>::option(const First &first, Args &&...args) : _async_runtime::OptionImplement<T>(ref<T>(first, std::forward<Args>(args)...)) {}

template <typename T>
bool _async_runtime::OptionImplement<T>::isNotNull(ref<T> &object) const
{
    const auto ptr = static_cast<const std::shared_ptr<T> &>(*this);
    if (ptr != nullptr)
    {
        object = ptr;
        return true;
    }
    else
        return false;
}

template <typename T>
ref<T> _async_runtime::OptionImplement<T>::isNotNullElse(std::function<ref<T>()> fn) const
{
    const auto ptr = static_cast<const std::shared_ptr<T> &>(*this);
    if (ptr != nullptr)
        return ref<T>(ptr);
    else
        return fn();
}

template <typename T>
ref<T> _async_runtime::OptionImplement<T>::assertNotNull() const
{
    const auto ptr = static_cast<const std::shared_ptr<T> &>(*this);
    if (ptr != nullptr)
        return ref<T>(ptr);
    else
        throw std::runtime_error(std::string(typeid(*this).name()) + " assert not null on a null ref. ");
}

namespace std
{
    template <typename T>
    struct hash<::option<T>>
    {
        std::size_t operator()(const ::option<T> &other) const { return other.hashCode(); }
    };

    template <typename T>
    struct hash<::ref<T>>
    {
        std::size_t operator()(const ::ref<T> &other) const
        {
            static const auto hs = hash<::option<T>>();
            return hs(static_cast<const ::option<T> &>(other));
        }
    };
} // namespace std
