#pragma once

#include "string_static.h"

class String::View : public String::Static
{
    using super = String::Static;

public:
    const ref<String> parent;
    View(ref<String> parent, const char *const pointer, const size_t len);
    ref<String> substr(size_t begin = 0, size_t length = SIZE_MAX) const override;
};
