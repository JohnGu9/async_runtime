#include "async_runtime/basic/duration.h"

Duration Duration::fromDetail(
    int days,
    int hours,
    int minutes,
    int seconds,
    int milliseconds,
    int microseconds)
{
    return Duration(millisecondsPerDay * days +
                    millisecondsPerHour * hours +
                    millisecondsPerMinute * minutes +
                    millisecondsPerSecond * seconds +
                    milliseconds);
}

Duration Duration::fromMilliseconds(int milliseconds)
{
    return Duration(milliseconds);
}

Duration Duration::fromSeconds(int seconds)
{
    return Duration(millisecondsPerSecond * seconds);
}

Duration Duration::fromMinutes(int minutes)
{
    return Duration(millisecondsPerMinute * minutes);
}

Duration Duration::fromHours(int hours)
{
    return Duration(millisecondsPerHour * hours);
}

Duration Duration::fromDays(int days)
{
    return Duration(millisecondsPerDay * days);
}

Duration::Duration(int64_t milliseconds) : _duration(microsecondsPerMillisecond * milliseconds) {}

static inline int64_t _abs(int64_t x) { return x >= 0 ? x : -x; }

Duration Duration::abs() const
{
    return Duration(_abs(this->_duration));
}

std::chrono::milliseconds Duration::toChronoMilliseconds() const
{
    return std::chrono::milliseconds(this->_duration / 1000);
}