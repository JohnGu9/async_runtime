#include "framework/fundamental/timer.h"

Timer::Timer(StatefulWidget::State *state) : Dispatcher(state) {}

Timer::~Timer()
{
    // Timer can automatic dispose itself
    if (!(*this->_clear))
        this->dispose();
}

void Timer::setTimeout(Fn<void()> function, Duration delay)
{
    Object::Ref<std::atomic_bool> clearFlag = Object::create<std::atomic_bool>(false);
    this->_clear = clearFlag;
    Object::Ref<Timer> self = Object::cast<Timer>(this); // hold a ref of self inside the Fn
    this->_threadPool->post([=]() {
        if (*clearFlag)
            return;
        std::this_thread::sleep_for(delay.toChronoMilliseconds());
        self->_handler->post([=] {
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
    Object::Ref<Timer> self = Object::cast<Timer>(this); // hold a ref of self inside the Fn
    this->_threadPool->post([=]() {
        while (true)
        {
            if (*clearFlag)
                return;
            std::this_thread::sleep_for(interval.toChronoMilliseconds());
            self->_handler->post([=] {
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