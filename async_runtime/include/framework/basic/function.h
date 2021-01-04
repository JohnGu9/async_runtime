#pragma once

#include <functional>
#include "../object.h"

class Function : public virtual Object
{
};

template <class _Fp>
class Fn;

template <class _Rp, class... _ArgTypes>
class Fn<_Rp(_ArgTypes...)> : public virtual Function, public std::function<_Rp(_ArgTypes...)>
{
public:
    Fn() : std::function<_Rp(_ArgTypes...)>() {}
    Fn(nullptr_t ptr) : std::function<_Rp(_ArgTypes...)>(ptr) {}
    Fn(std::function<_Rp(_ArgTypes...)> &fn) : std::function<_Rp(_ArgTypes...)>(fn) {}
    Fn(std::function<_Rp(_ArgTypes...)> &&fn) : std::function<_Rp(_ArgTypes...)>(std::forward<std::function<_Rp(_ArgTypes...)>>(fn)) {}
    Fn(_Rp (*fn)(_ArgTypes...)) : std::function<_Rp(_ArgTypes...)>(fn) {}
};
