#include <ctime>
#include "async_runtime/fundamental/async.h"
#include "async_runtime/fundamental/timer.h"

Timer::Timer(State<StatefulWidget> *state) : _callbackHandler(getHandlerfromState(state)), _autoReleaseThreadPool(AutoReleaseThreadPool::factory(1)) {}

Timer::Timer(Object::Ref<ThreadPool> callbackHandler) : _callbackHandler(callbackHandler), _autoReleaseThreadPool(AutoReleaseThreadPool::factory(1)) {}

Object::Ref<Timer> Timer::delay(State<StatefulWidget> *state, Duration duration, Function<void()> fn)
{
    Object::Ref<Timer> timer = Object::create<Timer>(state);
    timer->setTimeout(duration, fn);
    return timer;
}

Object::Ref<Timer> Timer::periodic(State<StatefulWidget> *state, Duration interval, Function<void()> fn)
{
    Object::Ref<Timer> timer = Object::create<Timer>(state);
    timer->setInterval(interval, fn);
    return timer;
}

Object::Ref<Timer> Timer::delay(Object::Ref<ThreadPool> callbackHandler, Duration duration, Function<void()> fn)
{
    Object::Ref<Timer> timer = Object::create<Timer>(callbackHandler);
    timer->setTimeout(duration, fn);
    return timer;
}

Object::Ref<Timer> Timer::periodic(Object::Ref<ThreadPool> callbackHandler, Duration interval, Function<void()> fn)
{
    Object::Ref<Timer> timer = Object::create<Timer>(callbackHandler);
    timer->setInterval(interval, fn);
    return timer;
}

Timer::~Timer()
{
    // Timer can automatic dispose itself
    if (not *(this->_clear))
        this->dispose();
}

void Timer::setTimeout(Duration delay, Function<void()> function)
{
    using std::chrono::system_clock;
    system_clock::time_point nextTime = system_clock::now();
    std::shared_ptr<std::atomic_bool> clearFlag = std::make_shared<std::atomic_bool>(false);
    this->_clear = clearFlag;
    Object::Ref<Timer> self = Object::cast<>(this); // hold a ref of self inside the Function
    this->_autoReleaseThreadPool->detach();
    this->_autoReleaseThreadPool = AutoReleaseThreadPool::factory(1);
    this->_autoReleaseThreadPool->post([=]() {
        if (*clearFlag)
            return;
        std::this_thread::sleep_until(nextTime + delay.toChronoMilliseconds());
        if (*clearFlag)
            return;
        self->_callbackHandler->microTask(function);
    });
}

void Timer::setInterval(Duration interval, Function<void()> function)
{
    using std::chrono::system_clock;
    system_clock::time_point nextTime = system_clock::now();
    Object::Ref<std::atomic_bool> clearFlag = std::make_shared<std::atomic_bool>(false);
    this->_clear = clearFlag;
    Object::Ref<Timer> self = Object::cast<>(this); // hold a ref of self inside the Function
    this->_autoReleaseThreadPool->detach();
    this->_autoReleaseThreadPool = AutoReleaseThreadPool::factory(1);
    this->_autoReleaseThreadPool->post([=](system_clock::time_point nextTime) {
        while (true)
        {
            if (*clearFlag)
                return;
            nextTime += interval.toChronoMilliseconds();
            std::this_thread::sleep_until(nextTime);
            if (*clearFlag)
                return;
            self->_callbackHandler->microTask(function);
        }
    },
                                       nextTime);
}

void Timer::cancel()
{
    this->_clear->operator=(true);
}

void Timer::dispose()
{
    this->cancel();
    this->_callbackHandler = nullptr;
    this->_autoReleaseThreadPool = nullptr;
}