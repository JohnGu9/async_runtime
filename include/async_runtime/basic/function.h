#pragma once

#include <functional>
#include <type_traits>
#include <memory>

template <typename T = std::nullptr_t>
class Function;

template <>
class Function<std::nullptr_t>
{
public:
    virtual operator bool() { return false; }
};

template <typename ReturnType, class... Args>
class Function<ReturnType(Args...)> : public Function<std::nullptr_t>
{
    template <typename T>
    friend struct std::hash;

public:
    Function() {}
    Function(std::nullptr_t) {}
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
    std::shared_ptr<std::function<ReturnType(Args...)>> _fn;
};

namespace std
{
    template <typename ReturnType, class... Args>
    struct hash<Function<ReturnType(Args...)>>
    {
        std::size_t operator()(const Function<ReturnType(Args...)> &other) const
        {
            return hash<std::shared_ptr<std::function<ReturnType(Args...)>>>()(other._fn);
        }
    };
} // namespace std