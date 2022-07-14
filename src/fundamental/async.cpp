#include "async_runtime/fundamental/async.h"

/**
 * @brief AsyncSnapshot implement
 *
 */
std::ostream &operator<<(std::ostream &os, const AsyncSnapshot<>::ConnectionState::Value &value)
{
    return os << AsyncSnapshot<>::ConnectionState::toString(value);
}

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
        return "AsyncSnapshot<>::ConnectionState::none"_String;
    case AsyncSnapshot<>::ConnectionState::active:
        return "AsyncSnapshot<>::ConnectionState::active"_String;
    case AsyncSnapshot<>::ConnectionState::done:
        return "AsyncSnapshot<>::ConnectionState::done"_String;
    default:
        throw NotImplementedError();
    }
}
