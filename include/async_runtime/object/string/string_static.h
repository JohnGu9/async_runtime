#pragma once

#include "string.h"

class String::Static : public String
{
public:
    const char *const pointer;
    const size_t len;
    Static(const char *const pointer, const size_t len) : pointer(pointer), len(len){};
    const char *const data() const override { return pointer; }
    size_t size() const override { return len; }
};
