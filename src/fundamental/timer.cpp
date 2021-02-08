#include <ctime>
#include "async_runtime/fundamental/async.h"
#include "async_runtime/fundamental/timer.h"

Timer::Timer(State<StatefulWidget> *state, const _CreateOnly &) : Dispatcher(state) {}

Timer::Timer(Object::Ref<ThreadPool> callbackHandler, const _CreateOnly &) : Dispatcher(callbackHandler) {}

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
    if (this->_clear != nullptr)
        this->dispose();
}

void Timer::_setTimeout(Duration delay, Function<void()> function)
{
    using std::chrono::system_clock;
    system_clock::time_point nextTime = system_clock::now();
    std::shared_ptr<std::atomic_bool> clearFlag = std::make_shared<std::atomic_bool>(false);
    this->_clear = clearFlag;
    Object::Ref<Timer> self = Object::cast<>(this); // hold a ref of self inside the Function
    this->post([=](Function<void(Function<void()>)> runJob) {
        if (*clearFlag)
            return;
        std::this_thread::sleep_until(nextTime + delay.toChronoMilliseconds());
        if (*clearFlag)
            return;
        runJob(function);
    });
}

void Timer::_setInterval(Duration interval, Function<void()> function)
{
    using std::chrono::system_clock;
    system_clock::time_point current = system_clock::now();
    Object::Ref<std::atomic_bool> clearFlag = std::make_shared<std::atomic_bool>(false);
    this->_clear = clearFlag;
    Object::Ref<Timer> self = Object::cast<>(this); // hold a ref of self inside the Function
    this->post([=](Function<void(Function<void()>)> runJob) {
        system_clock::time_point nextTime = current;
        while (true)
        {
            if (*clearFlag)
                return;
            nextTime = nextTime + interval.toChronoMilliseconds();
            std::this_thread::sleep_until(nextTime);
            if (*clearFlag)
                return;
            runJob(function);
        }
    });
}

void Timer::cancel()
{
    this->_clear->operator=(true);
}

void Timer::dispose()
{
    this->cancel();
    this->_clear = nullptr;
    Dispatcher::dispose();
}