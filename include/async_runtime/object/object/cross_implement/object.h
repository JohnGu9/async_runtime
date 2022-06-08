#pragma once
#include "../object.h"

/**
 * @brief create a object and return a ref to the object
 * all object should called from the function
 *
 * @tparam T
 * @tparam _Args
 * @param __args the constructor arguments
 * @return ref<T>
 */
template <typename T, typename std::enable_if<std::is_base_of<Object, T>::value>::type *, class... _Args>
ref<T> Object::create(_Args &&...__args)
{
    finalref<T> object = std::make_shared<T>(std::forward<_Args>(__args)...);
    object->init();
    return object;
}

/**
 * @brief detach let ref force to release the refences of object,
 * object may not dispose if there is any other ref of itself,
 * be careful to use this function that may cause serious problem.
 *
 * @param object the ref need to force to release (just like python del operator)
 */
template <typename T, typename std::enable_if<std::is_base_of<Object, T>::value>::type *>
void Object::detach(ref<T> &object) noexcept
{
    static_cast<std::shared_ptr<T> &>(object) = nullptr;
}

/**
 * @brief check two ref whether point to same object.
 */
template <typename T0, typename T1>
bool Object::identical(const option<T0> &lhs, const option<T1> &rhs) noexcept
{
    return static_cast<const std::shared_ptr<T0> &>(lhs) ==
           static_cast<const std::shared_ptr<T1> &>(rhs);
}

/**
 * @brief check two ref whether point to same object.
 */
template <typename T0, typename T1>
bool Object::identical(const ref<T0> &lhs, const ref<T1> &rhs) noexcept
{
    return static_cast<const std::shared_ptr<T0> &>(lhs) ==
           static_cast<const std::shared_ptr<T1> &>(rhs);
}

/**
 * @brief check two ref whether point to same object.
 */
template <typename T0, typename T1>
bool Object::identical(const ref<T0> &lhs, const option<T1> &rhs) noexcept
{
    return static_cast<const std::shared_ptr<T0> &>(lhs) ==
           static_cast<const std::shared_ptr<T1> &>(rhs);
}

/**
 * @brief check two ref whether point to same object.
 */
template <typename T0, typename T1>
bool Object::identical(const option<T0> &lhs, const ref<T1> &rhs) noexcept
{
    return static_cast<const std::shared_ptr<T0> &>(lhs) ==
           static_cast<const std::shared_ptr<T1> &>(rhs);
}

/**
 * @brief check two ref whether equal or not (the underlay implement of operator==(...));
 * 1) if two ref point to same object, it must return true;
 * 2) if two ref point to different objects, it will return the value from object0->operator(object1).
 */
template <typename T0, typename T1,
          typename std::enable_if<std::is_base_of<Object, T0>::value>::type *,
          typename std::enable_if<std::is_base_of<Object, T1>::value>::type *>
bool Object::equal(const option<T0> &lhs, const option<T1> &rhs)
{
    if (Object::identical<>(lhs, rhs))
        return true;
    else if (!Object::isNull<>(lhs) && !Object::isNull<>(rhs))
        return lhs.get()->operator==(Object::cast<>(rhs.get()));
    return false;
}

/**
 * @brief check two ref whether equal or not (the underlay implement of operator==(...));
 * 1) if two ref point to same object, it must return true;
 * 2) if two ref point to different objects, it will return the value from object0->operator(object1).
 */
template <typename T0, typename T1,
          typename std::enable_if<std::is_base_of<Object, T0>::value>::type *,
          typename std::enable_if<std::is_base_of<Object, T1>::value>::type *>
bool Object::equal(const ref<T0> &lhs, const ref<T1> &rhs)
{
    if (Object::identical<>(lhs, rhs))
        return true;
    return lhs.get()->operator==(rhs);
}

/**
 * @brief check two ref whether equal or not (the underlay implement of operator==(...));
 * 1) if two ref point to same object, it must return true;
 * 2) if two ref point to different objects, it will return the value from object0->operator(object1).
 */
template <typename T0, typename T1,
          typename std::enable_if<std::is_base_of<Object, T0>::value>::type *,
          typename std::enable_if<std::is_base_of<Object, T1>::value>::type *>
bool Object::equal(const ref<T0> &lhs, const option<T1> &rhs)
{
    if (Object::identical<>(lhs, rhs))
        return true;
    else if (Object::isNull<>(rhs))
        return false;
    return lhs.get()->operator==(Object::cast<>(rhs.get()));
}

/**
 * @brief check two ref whether equal or not (the underlay implement of operator==(...));
 * 1) if two ref point to same object, it must return true;
 * 2) if two ref point to different objects, it will return the value from object0->operator(object1).
 */
template <typename T0, typename T1,
          typename std::enable_if<std::is_base_of<Object, T0>::value>::type *,
          typename std::enable_if<std::is_base_of<Object, T1>::value>::type *>
bool Object::equal(const option<T0> &lhs, const ref<T1> &rhs)
{
    if (Object::identical<>(lhs, rhs))
        return true;
    else if (Object::isNull<>(lhs))
        return false;
    return lhs.get()->operator==(rhs);
}

/**
 * @brief check option whether point to nullptr.
 */
template <typename T>
bool Object::isNull(const option<T> &object) noexcept
{
    return static_cast<const std::shared_ptr<T> &>(object) == nullptr;
}

/**
 * @brief check option whether point to nullptr.
 */
template <typename T>
bool Object::isNull(option<T> &&object) noexcept
{
    return static_cast<const std::shared_ptr<T> &>(object) == nullptr;
}

/**
 * @brief safety downcast a raw point,
 * make sure point is not nullptr before use the function.
 *
 * @tparam T target type, downcast type
 * @tparam R origin type
 * @param other
 * @return ref<T>
 */
template <typename T, typename R, typename std::enable_if<std::is_base_of<T, R>::value>::type *>
ref<T> Object::cast(R *other)
{
    DEBUG_ASSERT(other != nullptr && "Object::cast on a nullptr pointer");
    return std::shared_ptr<T>(static_cast<std::enable_shared_from_this<Object> *>(other)->shared_from_this(), static_cast<T *>(other));
}

/**
 * @brief convert raw point to ref<T>,
 * make sure point is not nullptr before use the function,
 * used by [self()] marco.
 *
 * @param other
 * @return ref<T>
 */
template <typename T, typename std::enable_if<std::is_base_of<Object, T>::value>::type *>
ref<T> Object::cast(T *other)
{
    DEBUG_ASSERT(other != nullptr && "Object::cast on a nullptr pointer");
    return std::shared_ptr<T>(static_cast<std::enable_shared_from_this<Object> *>(other)->shared_from_this(), other);
}

/**
 * @brief safety upcast to type<T>;
 * if upcast failed< option point to nullptr;
 * if success, point to upcast-ed object.
 *
 * @tparam T
 * @return option<T>
 */
template <typename T>
option<T> Object::cast() noexcept
{
    return std::dynamic_pointer_cast<T>(this->std::enable_shared_from_this<Object>::shared_from_this());
}

/**
 * @brief unsafety upcast to type<T>;
 * if upcast failed, throw std::runtime_error;
 * if success, ref point to upcast-ed object.
 *
 * @tparam T
 * @return ref<T>
 */
template <typename T>
ref<T> Object::covariant() noexcept(false)
{
    if (T *castedPointer = dynamic_cast<T *>(this))
        return Object::cast<>(castedPointer);
    else
    {
        std::stringstream ss;
        ss << "Invalid type covariant from [" << typeid(*this).name() << "] to [" << typeid(T).name() << "]";
        throw std::runtime_error(ss.str());
    }
}
