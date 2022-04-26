/**
 * @file duration.h
 * @brief
 * Duration work for time duration in async runtime
 * Mostly for Timer
 *
 * Duration is not Object that not a part of nullsafety system
 * Do not use ref<Duration> or option <Duration>
 *
 */

#pragma once

#if __APPLE__
// apple <cmath> include bug
#include <cstdlib>
#else
#include <cmath>
#endif

#include <assert.h>
#include <chrono>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

/**
 * @brief Duration for a peroid of time in Async Runtime
 *
 * @alarm native copyable type that don't not need ref
 * @alarm only support minimum unit milliseconds
 */
class Duration
{
    using DataType = signed long long;

public:
    static const DataType microsecondsPerMillisecond = 1000;
    static const DataType millisecondsPerSecond = 1000;
    static const DataType secondsPerMinute = 60;
    static const DataType minutesPerHour = 60;
    static const DataType hoursPerDay = 24;

    static const DataType microsecondsPerSecond =
        microsecondsPerMillisecond * millisecondsPerSecond;
    static const DataType microsecondsPerMinute =
        microsecondsPerSecond * secondsPerMinute;
    static const DataType microsecondsPerHour = microsecondsPerMinute * minutesPerHour;
    static const DataType microsecondsPerDay = microsecondsPerHour * hoursPerDay;

    static const DataType millisecondsPerMinute =
        millisecondsPerSecond * secondsPerMinute;
    static const DataType millisecondsPerHour = millisecondsPerMinute * minutesPerHour;
    static const DataType millisecondsPerDay = millisecondsPerHour * hoursPerDay;

    static const DataType secondsPerHour = secondsPerMinute * minutesPerHour;
    static const DataType secondsPerDay = secondsPerHour * hoursPerDay;

    static const DataType minutesPerDay = minutesPerHour * hoursPerDay;

    static Duration fromDetail(
        signed long long days = 0,
        signed long long hours = 0,
        signed long long minutes = 0,
        signed long long seconds = 0,
        signed long long milliseconds = 0,
        signed long long microseconds = 0 /* Duration only support milliseconds, microseconds mean nothing */);

    static Duration fromMilliseconds(signed long long milliseconds);
    static Duration fromSeconds(signed long long seconds);
    static Duration fromMinutes(signed long long minutes);
    static Duration fromHours(signed long long hours);
    static Duration fromDays(signed long long days);

    Duration(signed long long milliseconds);

    Duration operator+(Duration &&other) const
    {
        return Duration(_duration + other._duration);
    }

    /**
     * Subtracts [other] from this Duration and
     * returns the difference as a new Duration object.
     */
    Duration operator-(Duration &&other) const
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
    Duration operator*(DataType factor) const
    {
        return Duration(_duration * factor);
    }

    Duration operator*(double factor) const
    {
        double result = factor * _duration;
        return Duration(std::llround(result));
    }

    /**
     * Divides this Duration by the given [quotient] and returns the truncated
     * result as a new Duration object.
     *
     * Throws an [IntegerDivisionByZeroException] if [quotient] is `0`.
     */
    Duration operator/(int quotient) const
    {
        // By doing the check here instead of relying on "~/" below we get the
        // exception even with dart2js.
        assert(quotient != 0);
        return Duration(std::llround(_duration / quotient));
    }

    /**
     * Returns `true` if the value of this Duration
     * is less than the value of [other].
     */
    bool operator<(Duration other) const { return this->_duration < other._duration; }

    /**
     * Returns `true` if the value of this Duration
     * is greater than the value of [other].
     */
    bool operator>(Duration other) const { return this->_duration > other._duration; }

    /**
     * Returns `true` if the value of this Duration
     * is less than or equal to the value of [other].
     */
    bool operator<=(Duration other) const { return this->_duration <= other._duration; }

    /**
     * Returns `true` if the value of this Duration
     * is greater than or equal to the value of [other].
     */
    bool operator>=(Duration other) const { return this->_duration >= other._duration; }

    Duration abs() const;

    std::chrono::milliseconds toChronoMilliseconds() const;
    DataType toMilliseconds() const { return _duration / 1000; }

protected:
    DataType _duration;
};