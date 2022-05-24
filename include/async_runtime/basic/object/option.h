#pragma once
#include "option_implement.h"
#include "ref.h"

template <typename T>
class option : public _async_runtime::OptionImplement<T>
{
    _ASYNC_RUNTIME_FRIEND_FAMILY;

    template <typename R>
    friend bool operator==(const option<R> &opt, std::nullptr_t);
    template <typename R>
    friend bool operator!=(const option<R> &opt, std::nullptr_t);

    template <typename X, typename Y>
    friend bool operator==(const option<X> &object0, option<Y> object1);
    template <typename X, typename Y>
    friend bool operator!=(const option<X> &object0, option<Y> object1);

    template <typename X, typename Y>
    friend bool operator==(const option<X> &object0, ref<Y> object1);
    template <typename X, typename Y>
    friend bool operator!=(const option<X> &object0, ref<Y> object1);

    template <typename X, typename Y>
    friend bool operator==(const ref<X> &object0, option<Y> object1);
    template <typename X, typename Y>
    friend bool operator!=(const ref<X> &object0, option<Y> object1);

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
