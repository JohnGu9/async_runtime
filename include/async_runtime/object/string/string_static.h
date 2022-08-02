#pragma once
#include "string.h"

class String::Static : public String
{
public:
    const char *const pointer;
    const size_t len;
    Static(const char *const pointer, const size_t len) noexcept : pointer(std::move(pointer)), len(std::move(len)){};
    const char *const data() const override { return pointer; }
    size_t size() const override { return len; }
};
