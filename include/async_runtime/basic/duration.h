/// Duration work for time duration in async runtime
/// Mostly for Timer
///
/// Duration is not Object that not a part of nullsafety system
/// Do not use ref<Duration> or option <Duration>
///

#pragma once

#if __APPLE__
// apple <cmath> include bug
#include <cstdlib>
#else
#include <cmath>
#endif

#include <chrono>
#include <string>
#include <memory>
#include <sstream>
#include <assert.h>
#include <unordered_map>
#include <vector>

// native copyable type that don't not need ref
class Duration
{
public:
    static const int64_t microsecondsPerMillisecond = 1000;
    static const int64_t millisecondsPerSecond = 1000;
    static const int64_t secondsPerMinute = 60;
    static const int64_t minutesPerHour = 60;
    static const int64_t hoursPerDay = 24;

    static const int64_t microsecondsPerSecond =
        microsecondsPerMillisecond * millisecondsPerSecond;
    static const int64_t microsecondsPerMinute =
        microsecondsPerSecond * secondsPerMinute;
    static const int64_t microsecondsPerHour = microsecondsPerMinute * minutesPerHour;
    static const int64_t microsecondsPerDay = microsecondsPerHour * hoursPerDay;

    static const int64_t millisecondsPerMinute =
        millisecondsPerSecond * secondsPerMinute;
    static const int64_t millisecondsPerHour = millisecondsPerMinute * minutesPerHour;
    static const int64_t millisecondsPerDay = millisecondsPerHour * hoursPerDay;

    static const int64_t secondsPerHour = secondsPerMinute * minutesPerHour;
    static const int64_t secondsPerDay = secondsPerHour * hoursPerDay;

    static const int64_t minutesPerDay = minutesPerHour * hoursPerDay;

    static Duration fromDetail(
        int days = 0,
        int hours = 0,
        int minutes = 0,
        int seconds = 0,
        int milliseconds = 0,
        int microseconds = 0);

    static Duration fromMilliseconds(int milliseconds);
    static Duration fromSeconds(int seconds);
    static Duration fromMinutes(int minutes);
    static Duration fromHours(int hours);
    static Duration fromDays(int days);

    Duration(int64_t milliseconds);

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
    Duration operator*(int64_t factor) const
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

protected:
    int64_t _duration;
};