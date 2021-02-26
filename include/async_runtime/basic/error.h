#pragma once

#include "../object.h"
#include "string.h"

class Error : public Object
{
public:
    Error(String message) : message(message) {}
    Error(String message, ref<Error> from) : from(from), message(message) {}
    Error(const Error &other) : from(other.from), message(other.message) {}

    const ref<Error> from;
    const String message;

    void toStringStream(std::ostream &os) override;
};