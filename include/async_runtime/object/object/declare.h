#pragma once
#include <functional>
#include <memory>
#include <type_traits>

#include "declare/basic.h"
#include "declare/friend.h"
#include "declare/operators.h"

#include "declare/function.h"
#include "declare/string.h"

#include "declare/internal.h"

#define finalref const ref
#define finaloption const option
#define self() ::Object::cast<>(this)
#define constSelf() ::Object::cast<>(const_cast<typename ::_async_runtime::RemovePointerConst<decltype(this)>::type>(this))

#ifdef NDEBUG
#define DEBUG_ASSERT(...) void(0)
#else
#include <assert.h>
#define DEBUG_ASSERT(...) assert(__VA_ARGS__)
#endif

#include <exception>
#define RUNTIME_ASSERT(__statement__, __description__)   \
    {                                                    \
        if (!(__statement__))                            \
            throw std::runtime_error((__description__)); \
    }

class NotImplementedError : public std::exception
{
public:
    NotImplementedError() noexcept : std::exception(){};
    const char *what() const noexcept override { return "Function not yet implemented"; }
};
