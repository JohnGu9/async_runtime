#pragma once
#include "ref.h"

// non-nullable object
template <typename T>
class lateref : public ref<T>
{
    _ASYNC_RUNTIME_FRIEND_FAMILY;
    using super = ref<T>;

public:
    explicit lateref() : super() {}
    lateref(std::nullptr_t) = delete;

    template <typename R, typename std::enable_if<std::is_base_of<T, R>::value>::type * = nullptr>
    lateref(const ref<R> &other) : super(other) {}
    template <typename R, typename std::enable_if<std::is_base_of<T, R>::value>::type * = nullptr>
    lateref(ref<R> &&other) : super(std::move(other)) {}

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
    lateref(const First &first, Args &&...args);

#ifndef NDEBUG

    T &operator*() const
    {
        assert(super::get() && "ref Uninitiated NullReference Error! As usually this error cause by lateref that use before assgin a non-null reference. ");
        return super::operator*();
    }
    T *operator->() const
    {
        assert(super::get() && "ref Uninitiated NullReference Error! As usually this error cause by lateref that use before assgin a non-null reference. ");
        return super::operator->();
    }

#endif
};
