#pragma once

#include <functional>
#include <type_traits>
#include <memory>

template <typename T>
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
    static_assert(std::is_constructible<std::function<ReturnType(Args...)>, ReturnType(Args...)>::value, "Can't construct object in function");

    template <typename T>
    friend struct std::hash;

public:
    Function() {}
    Function(std::nullptr_t) {}
    Function(const Function &other) : _fn(other._fn) {}
    template <typename Lambda, typename std::enable_if<std::is_constructible<std::function<ReturnType(Args...)>, Lambda>::value>::type * = nullptr>
    Function(Lambda lambda) : _fn(std::make_shared<std::function<ReturnType(Args...)>>(lambda)) {}

    virtual Function &operator=(std::nullptr_t)
    {
        this->_fn = nullptr;
        return *this;
    }

    virtual ReturnType operator()(Args... args) const { return _fn->operator()(std::forward<Args>(args)...); }
    virtual bool operator==(const Function &other) const { return _fn == other._fn; }
    virtual bool operator!=(const Function &other) const { return _fn != other._fn; }
    virtual bool operator==(std::nullptr_t) const { return _fn == nullptr; }
    virtual bool operator!=(std::nullptr_t) const { return _fn != nullptr; }

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