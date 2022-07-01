#pragma once

/**
 * @brief Function object in Async Runtime (raw version)
 *
 */
template <typename T>
class Fn;

/**
 * @brief Function object in Async Runtime
 *
 *
 * @tparam T
 * the function format
 *
 * @example
 * Function<void()> fn = []() -> void {};
 *
 */
template <typename T>
using Function = ref<Fn<T>>;
