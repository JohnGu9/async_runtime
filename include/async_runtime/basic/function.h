#pragma once

#include <functional>
#include <type_traits>

#include "../object.h"

template <typename T = nullptr_t>
class Function;

template <>
class Function<nullptr_t> : public virtual Object
{
public:
    virtual operator bool() { return false; }
};

template <typename ReturnType, class... Args>
class Function<ReturnType(Args...)> : public Function<nullptr_t>
{
public:
    Function() {}
    Function(nullptr_t) {}
    Function(const Function &other) : _fn(other._fn) {}
    Function(std::function<ReturnType(Args...)> fn)
        : _fn(std::make_shared<std::function<ReturnType(Args...)>>(fn)) { assert(fn); }
    template <typename Lambda>
    Function(Lambda lambda) : _fn(std::make_shared<std::function<ReturnType(Args...)>>(lambda)) {}

    virtual ReturnType operator()(Args... args) const { return _fn->operator()(std::forward<Args>(args)...); }
    virtual bool operator==(const Function &other) const { return _fn == other._fn; }
    virtual size_t key() const
    {
        if (_fn)
            return (size_t)_fn.get();
        return 0;
    }

    operator bool() override { return _fn != nullptr; }

protected:
    Object::Ref<std::function<ReturnType(Args...)>> _fn;
};
