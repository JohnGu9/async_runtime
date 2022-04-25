#include "async_runtime/basic/duration.h"

Duration Duration::fromDetail(
    DataType days,
    DataType hours,
    DataType minutes,
    DataType seconds,
    DataType milliseconds,
    DataType microseconds)
{
    return Duration(millisecondsPerDay * days +
                    millisecondsPerHour * hours +
                    millisecondsPerMinute * minutes +
                    millisecondsPerSecond * seconds +
                    milliseconds);
}

Duration Duration::fromMilliseconds(DataType milliseconds)
{
    return Duration(milliseconds);
}

Duration Duration::fromSeconds(DataType seconds)
{
    return Duration(millisecondsPerSecond * seconds);
}

Duration Duration::fromMinutes(DataType minutes)
{
    return Duration(millisecondsPerMinute * minutes);
}

Duration Duration::fromHours(DataType hours)
{
    return Duration(millisecondsPerHour * hours);
}

Duration Duration::fromDays(DataType days)
{
    return Duration(millisecondsPerDay * days);
}

Duration::Duration(DataType milliseconds) : _duration(microsecondsPerMillisecond * milliseconds) {}

static inline signed long long _abs(signed long long x) { return x >= 0 ? x : -x; }

Duration Duration::abs() const
{
    return Duration(_abs(this->_duration));
}

std::chrono::milliseconds Duration::toChronoMilliseconds() const
{
    return std::chrono::milliseconds(toMilliseconds());
}