#pragma once
#include <cmath>

/**
 * @brief Duration for a period of time in Async Runtime
 * @warning native copyable type that don't not need ref
 *
 */
class Duration
{
    using DataType = signed long long;

public:
    constexpr static const DataType microsecondsPerMillisecond = 1000;
    constexpr static const DataType millisecondsPerSecond = 1000;
    constexpr static const DataType secondsPerMinute = 60;
    constexpr static const DataType minutesPerHour = 60;
    constexpr static const DataType hoursPerDay = 24;

    constexpr static const DataType microsecondsPerSecond =
        microsecondsPerMillisecond * millisecondsPerSecond;
    constexpr static const DataType microsecondsPerMinute =
        microsecondsPerSecond * secondsPerMinute;
    constexpr static const DataType microsecondsPerHour = microsecondsPerMinute * minutesPerHour;
    constexpr static const DataType microsecondsPerDay = microsecondsPerHour * hoursPerDay;

    constexpr static const DataType millisecondsPerMinute =
        millisecondsPerSecond * secondsPerMinute;
    constexpr static const DataType millisecondsPerHour = millisecondsPerMinute * minutesPerHour;
    constexpr static const DataType millisecondsPerDay = millisecondsPerHour * hoursPerDay;

    constexpr static const DataType secondsPerHour = secondsPerMinute * minutesPerHour;
    constexpr static const DataType secondsPerDay = secondsPerHour * hoursPerDay;

    constexpr static const DataType minutesPerDay = minutesPerHour * hoursPerDay;

    constexpr static Duration fromDetail(
        signed long long days = 0,
        signed long long hours = 0,
        signed long long minutes = 0,
        signed long long seconds = 0,
        signed long long milliseconds = 0,
        signed long long microseconds = 0);

    constexpr static Duration fromMilliseconds(signed long long milliseconds);
    constexpr static Duration fromSeconds(signed long long seconds);
    constexpr static Duration fromMinutes(signed long long minutes);
    constexpr static Duration fromHours(signed long long hours);
    constexpr static Duration fromDays(signed long long days);

    constexpr Duration(signed long long milliseconds);

    constexpr Duration operator+(const Duration &other) const
    {
        return Duration(_duration + other._duration);
    }

    /**
     * Subtracts [other] from this Duration and
     * returns the difference as a new Duration object.
     */
    constexpr Duration operator-(const Duration &other) const
    {
        return Duration(_duration - other._duration);
    }

    /**
     * Multiplies this Duration by the given [factor] and returns the result
     * as a new Duration object.
     *
     * Note that when [factor] is a double, and the duration is greater than
     * 53 bits, precision is lost because of double-precision arithmetic.
     */
    constexpr Duration operator*(size_t factor) const
    {
        return Duration(_duration * factor);
    }

    Duration operator*(double factor) const
    {
        return Duration(std::llround(factor * _duration));
    }

    /**
     * Divides this Duration by the given [quotient] and returns the truncated
     * result as a new Duration object.
     *
     * Throws an [IntegerDivisionByZeroException] if [quotient] is `0`.
     */
    Duration operator/(double quotient) const
    {
        return Duration(std::llround(_duration / quotient));
    }

    /**
     * Returns `true` if the value of this Duration
     * is less than the value of [other].
     */
    constexpr bool operator<(const Duration &other) const { return this->_duration < other._duration; }

    /**
     * Returns `true` if the value of this Duration
     * is greater than the value of [other].
     */
    constexpr bool operator>(const Duration &other) const { return this->_duration > other._duration; }

    /**
     * Returns `true` if the value of this Duration
     * is less than or equal to the value of [other].
     */
    constexpr bool operator<=(const Duration &other) const { return this->_duration <= other._duration; }

    /**
     * Returns `true` if the value of this Duration
     * is greater than or equal to the value of [other].
     */
    constexpr bool operator>=(const Duration &other) const { return this->_duration >= other._duration; }

    constexpr DataType toSeconds() const { return _duration / microsecondsPerSecond; }
    constexpr DataType toMilliseconds() const { return _duration / microsecondsPerMillisecond; }
    constexpr DataType toMicroseconds() const { return _duration; }

    Duration abs() const;

protected:
    const DataType _duration;
    constexpr Duration(DataType milliseconds, DataType microseconds);
};

constexpr inline Duration::Duration(DataType milliseconds) : _duration(microsecondsPerMillisecond * milliseconds) {}
constexpr inline Duration::Duration(DataType milliseconds, DataType microseconds) : _duration(microseconds) {}

constexpr inline Duration Duration::fromDetail(
    DataType days,
    DataType hours,
    DataType minutes,
    DataType seconds,
    DataType milliseconds,
    DataType microseconds)
{
    return Duration(0, microsecondsPerDay * days +
                           microsecondsPerHour * hours +
                           microsecondsPerMinute * minutes +
                           microsecondsPerSecond * seconds +
                           microsecondsPerMillisecond * milliseconds +
                           microseconds);
}

constexpr inline Duration Duration::fromMilliseconds(DataType milliseconds)
{
    return Duration(milliseconds);
}

constexpr inline Duration Duration::fromSeconds(DataType seconds)
{
    return Duration(millisecondsPerSecond * seconds);
}

constexpr inline Duration Duration::fromMinutes(DataType minutes)
{
    return Duration(millisecondsPerMinute * minutes);
}

constexpr inline Duration Duration::fromHours(DataType hours)
{
    return Duration(millisecondsPerHour * hours);
}

constexpr inline Duration Duration::fromDays(DataType days)
{
    return Duration(millisecondsPerDay * days);
}

inline Duration Duration::abs() const // in Linux system, std::abs is not constexpr
{
    return Duration(std::abs(this->_duration));
}
