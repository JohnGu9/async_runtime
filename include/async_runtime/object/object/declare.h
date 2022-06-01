#pragma once

#include <assert.h>
#include <exception>
#include <functional>
#include <memory>

#include "declare/basic.h"
#include "declare/friend.h"
#include "declare/operators.h"

#include "declare/function.h"
#include "declare/string.h"

#include "declare/internal.h"

#define finalref const ref
#define finaloption const option
#define self() Object::cast<>(this)

#ifdef NDEBUG
#define DEBUG_ASSERT(...) void(0)
#else
#define DEBUG_ASSERT(...) assert(__VA_ARGS__)
#endif

#include <exception>
#define RUNTIME_ASSERT(__statement__, __description__)   \
    {                                                    \
        if (!(__statement__))                            \
            throw std::runtime_error((__description__)); \
    }
