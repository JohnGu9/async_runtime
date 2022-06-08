/**
 * @brief
 * The mechanism of nullsafety system
 * Implement base on std::shared_ptr and std::weak_ptr (don't be confused by std::ref)
 * Thread safe and memory safe
 *
 *
 * inherits layout:
 *
 * std::shared_ptr
 *       ^
 *       |
 *  _async_runtime::OptionImplement
 *       ^
 *       |
 *  option
 *
 *
 * std::shared_ptr
 *       ^
 *       |
 *  _async_runtime::RefImplement
 *       ^
 *       |
 *  ref
 *       ^
 *       |
 *  lateref
 *
 *
 *  std::weak_ptr
 *       ^
 *       |
 *  weakref
 *
 *
 * @example
 * ref<Object> object = Object::create<Object>(); // create a object through Object::create
 * ref<Object> refOfObject = object; // another ref of object
 *
 * option<Object> opt = object; // nullable ref and lock the object resource
 * opt = nullptr; // release ref and unlock the resource
 *
 * weakref<Object> wr = object; // weak ref that point toward a ref that doesn't lock the resource
 *
 * refOfObject->function(); // only ref call directly call member. option and weakref can not.
 * // only way option and weakref call member is to change option and weakref to ref
 *
 *
 * // change option(/weakref) to ref through ToRefMixin api
 * object = opt.assertNotNull(); // assert is not a good option change that may cause error
 *
 * if_not_null (opt) { // null check is a good option
 *     // [opt] is not null here
 * }
 *
 * object = opt.ifNotNullElse([]() ->ref<Object> { return Object::create<Object>(); }) // if opt is not null change opt to ref, otherwise create a new Object
 * object = opt.ifNotNullElse(Object::create<Object>); another syntax
 *
 */

#pragma once
#include "object/declare.h"

#include "object/option_implement.h"
#include "object/ref_implement.h"

#include "object/ref.h"

#include "object/object.h"

#include "container.h"
#include "function.h"
#include "string.h"

#include "object/lateref.h"
#include "object/option.h"
#include "object/weak_ref.h"

#include "object/cross_implement.h"
#include "object/hash.h"
#include "object/less.h"
#include "object/operators.h"
