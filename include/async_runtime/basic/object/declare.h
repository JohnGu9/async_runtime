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
