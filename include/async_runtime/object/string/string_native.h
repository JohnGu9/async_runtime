#pragma once
#include "string.h"

class String::Native : public String, protected std::string
{
    using super = std::string;

public:
    using super::super;
    Native(const std::string &other) : super(other) {}
    Native(std::string &&other) : super(std::move(other)) {}

    // override from Iterable
    size_t size() const override { return this->std::string::size(); }
    // override from String
    const char *const data() const override { return this->std::string::data(); }
};
