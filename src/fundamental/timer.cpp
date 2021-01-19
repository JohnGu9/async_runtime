#include <ctime>
#include "framework/fundamental/timer.h"

Timer::Timer(StatefulWidget::State *state) : Dispatcher(state) {}

Object::Ref<Timer> Timer::delay(StatefulWidget::State *state, Fn<void()> fn, Duration duration)
{
    Object::Ref<Timer> timer = Object::create<Timer>(state);
    timer->setTimeout(fn, duration);
    return timer;
}

Object::Ref<Timer> Timer::periodic(StatefulWidget::State *state, Fn<void()> fn, Duration interval)
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

void Timer::setTimeout(Fn<void()> function, Duration delay)
{
    Object::Ref<std::atomic_bool> clearFlag = Object::create<std::atomic_bool>(false);
    this->_clear = clearFlag;
    Object::Ref<Timer> self = Object::cast<>(this); // hold a ref of self inside the Fn
    this->_threadPool->post([=]() {
        if (*clearFlag)
            return;
        std::this_thread::sleep_for(delay.toChronoMilliseconds());
        self->_handler->microTask([=] {
            if (*clearFlag)
                return;
            function();
        });
    });
}

void Timer::setInterval(Fn<void()> function, Duration interval)
{
    Object::Ref<std::atomic_bool> clearFlag = Object::create<std::atomic_bool>(false);
    this->_clear = clearFlag;
    Object::Ref<Timer> self = Object::cast<>(this); // hold a ref of self inside the Fn
    this->_threadPool->post([=]() {
        using std::chrono::system_clock;
        system_clock::time_point nextTime = system_clock::now();
        while (true)
        {
            if (*clearFlag)
                return;
            nextTime += interval.toChronoMilliseconds();
            std::this_thread::sleep_until(nextTime);
            self->_handler->microTask([=] {
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