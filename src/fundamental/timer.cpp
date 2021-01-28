#include <ctime>
#include "async_runtime/fundamental/async.h"
#include "async_runtime/fundamental/timer.h"

Timer::Timer(State<StatefulWidget> *state) : Dispatcher(state) {}

Object::Ref<Timer> Timer::delay(State<StatefulWidget> *state, Function<void()> fn, Duration duration)
{
    Object::Ref<Timer> timer = Object::create<Timer>(state);
    timer->setTimeout(fn, duration);
    return timer;
}

Object::Ref<Timer> Timer::periodic(State<StatefulWidget> *state, Function<void()> fn, Duration interval)
{
    Object::Ref<Timer> timer = Object::create<Timer>(state);
    timer->setInterval(fn, interval);
    return timer;
}

Timer::~Timer()
{
    // Timer can automatic dispose itself
    if (!(*(this->_clear)))
        this->dispose();
}

void Timer::setTimeout(Function<void()> function, Duration delay)
{
    std::shared_ptr<std::atomic_bool> clearFlag = std::make_shared<std::atomic_bool>(false);
    this->_clear = clearFlag;
    Object::Ref<Timer> self = Object::cast<>(this); // hold a ref of self inside the Function
    this->_threadPool->post([=]() {
        if (*clearFlag)
            return;
        std::this_thread::sleep_for(delay.toChronoMilliseconds());
        self->_callbackHandler->microTask([=] {
            if (*clearFlag)
                return;
            function();
        });
    });
}

void Timer::setInterval(Function<void()> function, Duration interval)
{
    Object::Ref<std::atomic_bool> clearFlag = std::make_shared<std::atomic_bool>(false);
    this->_clear = clearFlag;
    Object::Ref<Timer> self = Object::cast<>(this); // hold a ref of self inside the Function
    this->_threadPool->post([=]() {
        using std::chrono::system_clock;
        system_clock::time_point nextTime = system_clock::now();
        while (true)
        {
            if (*clearFlag)
                return;
            nextTime += interval.toChronoMilliseconds();
            std::this_thread::sleep_until(nextTime);
            self->_callbackHandler->microTask([=] {
                if (*clearFlag)
                    return;
                function();
            });
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
    Dispatcher::dispose();
}