#pragma once
#include "ref.h"

// non-nullable object
template <typename T>
class lateref : public ref<T>
{
    _ASYNC_RUNTIME_FRIEND_FAMILY;

public:
    explicit lateref() : ref<T>() {}
    lateref(std::nullptr_t) = delete;

    template <typename R, typename std::enable_if<std::is_base_of<T, R>::value>::type * = nullptr>
    lateref(const ref<R> &other) : ref<T>(other) {}

    template <typename R, typename std::enable_if<std::is_base_of<T, R>::value>::type * = nullptr>
    lateref(ref<R> &&other) : ref<T>(std::move(other)) {}

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
};
