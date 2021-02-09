#include <ctime>
#include "async_runtime/fundamental/async.h"
#include "async_runtime/fundamental/timer.h"

Timer::Timer(State<StatefulWidget> *state, const _CreateOnly &)
    : Dispatcher(state), _clear(false), _completed(false) {}

Timer::Timer(Object::Ref<ThreadPool> callbackHandler, const _CreateOnly &)
    : Dispatcher(callbackHandler), _clear(false), _completed(false) {}

Object::Ref<Timer> Timer::delay(State<StatefulWidget> *state, Duration duration, Function<void()> fn)
{
    static const _CreateOnly lock = _CreateOnly();
    Object::Ref<Timer> timer = Object::create<Timer>(state, lock);
    timer->_setTimeout(duration, fn);
    return timer;
}

Object::Ref<Timer> Timer::periodic(State<StatefulWidget> *state, Duration interval, Function<void()> fn)
{
    static const _CreateOnly lock = _CreateOnly();
    Object::Ref<Timer> timer = Object::create<Timer>(state, lock);
    timer->_setInterval(interval, fn);
    return timer;
}

Object::Ref<Timer> Timer::delay(Object::Ref<ThreadPool> callbackHandler, Duration duration, Function<void()> fn)
{
    Object::Ref<Timer> timer = Object::create<Timer>(callbackHandler, _CreateOnly());
    timer->_setTimeout(duration, fn);
    return timer;
}

Object::Ref<Timer> Timer::periodic(Object::Ref<ThreadPool> callbackHandler, Duration interval, Function<void()> fn)
{
    Object::Ref<Timer> timer = Object::create<Timer>(callbackHandler, _CreateOnly());
    timer->_setInterval(interval, fn);
    return timer;
}

Timer::~Timer()
{
    // Timer can automatic dispose itself
    this->dispose();
}

void Timer::_setTimeout(Duration delay, Function<void()> function)
{
    assert(this->_clear == false);
    using std::chrono::system_clock;
    system_clock::time_point current = system_clock::now();
    Object::Ref<Timer> self = Object::cast<>(this); // hold a ref of self inside the Function
    _thread = std::make_shared<Thread>([=] {
        if (self->_clear)
        {
            self->_completed = true;
            return;
        }
        std::this_thread::sleep_until(current + delay.toChronoMilliseconds());
        if (self->_clear)
        {
            self->_completed = true;
            return;
        }
        self->microTask(function);
    });
}

void Timer::_setInterval(Duration interval, Function<void()> function)
{
    assert(this->_clear == false);
    using std::chrono::system_clock;
    system_clock::time_point current = system_clock::now();
    Object::Ref<Timer> self = Object::cast<>(this); // hold a ref of self inside the Function
    _thread = std::make_shared<Thread>([=] {
        system_clock::time_point nextTime = current;
        if (self->_clear)
        {
            self->_completed = true;
            return;
        }
        for (;;)
        {
            nextTime += interval.toChronoMilliseconds();
            std::this_thread::sleep_until(nextTime);
            if (self->_clear)
            {
                self->_completed = true;
                return;
            }
            self->microTask(function);
        }
    });
}

void Timer::cancel()
{
    this->_clear = true;
}

void Timer::dispose()
{
    this->cancel();
    this->_thread->detach(); // timer would wait thread complete.
    Dispatcher::dispose();
}