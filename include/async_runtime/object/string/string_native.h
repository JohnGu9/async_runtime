#pragma once
#include "string.h"
#include "vector"

class String::Native : public String
{
    const std::string _container;

public:
    template <typename... Args, typename = decltype(std::string(std::declval<Args>()...))>
    Native(Args &&...args) : _container(std::forward<Args>(args)...) {}

    // override from Iterable
    size_t size() const override { return this->_container.size(); }
    // override from String
    const char *const data() const override { return this->_container.data(); }
};
