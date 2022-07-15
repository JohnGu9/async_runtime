#pragma once

#include "string.h"

class String::Native : public String, protected std::string
{
    using super = std::string;

public:
    Native(std::initializer_list<char> list) : super(std::move(list)) {}
    Native(const std::string &str) : super(str) {}
    Native(std::string &&str) : super(std::move(str)) {}
    Native(const char *const str) : super(str) { DEBUG_ASSERT(str != nullptr); }
    Native(const char *const str, size_t length) : super(str, length) { DEBUG_ASSERT(str != nullptr); }

    // override from Iterable
    size_t size() const override { return this->std::string::size(); }
    // override from String
    const char *const data() const override { return this->std::string::data(); }
};
