#include "async_runtime/fundamental/async.h"

/**
 * @brief AsyncSnapshot implement
 *
 */
const ref<List<AsyncSnapshot<>::ConnectionState::Value>>
    AsyncSnapshot<>::ConnectionState::values = {
        AsyncSnapshot<>::ConnectionState::none,
        AsyncSnapshot<>::ConnectionState::active,
        AsyncSnapshot<>::ConnectionState::done,
};

ref<String> AsyncSnapshot<>::ConnectionState::toString(AsyncSnapshot<>::ConnectionState::Value value)
{
    switch (value)
    {
    case AsyncSnapshot<>::ConnectionState::none:
        return "AsyncSnapshot<>::ConnectionState::none";
    case AsyncSnapshot<>::ConnectionState::active:
        return "AsyncSnapshot<>::ConnectionState::active";
    case AsyncSnapshot<>::ConnectionState::done:
        return "AsyncSnapshot<>::ConnectionState::done";
    default:
        RUNTIME_ASSERT(false, "The enum doesn't exists. ");
        return "The enum doesn't exists";
    }
}
