#include "async_runtime/fundamental/async.h"

/**
 * @brief AsyncSnapshot implement
 *
 */
std::ostream &operator<<(std::ostream &os, const AsyncSnapshot<>::ConnectionState::Value &value)
{
    switch (value)
    {
    case AsyncSnapshot<>::ConnectionState::none:
        return os << "AsyncSnapshot<>::ConnectionState::none";
    case AsyncSnapshot<>::ConnectionState::active:
        return os << "AsyncSnapshot<>::ConnectionState::active";
    case AsyncSnapshot<>::ConnectionState::done:
        return os << "AsyncSnapshot<>::ConnectionState::done";
    default:
        throw NotImplementedError();
    }
}

const ref<List<AsyncSnapshot<>::ConnectionState::Value>>
    AsyncSnapshot<>::ConnectionState::values = {
        AsyncSnapshot<>::ConnectionState::none,
        AsyncSnapshot<>::ConnectionState::active,
        AsyncSnapshot<>::ConnectionState::done,
};

ref<String> AsyncSnapshot<>::ConnectionState::toString(AsyncSnapshot<>::ConnectionState::Value value)
{
    std::stringstream ss;
    ss << value;
    return ss.str();
}
