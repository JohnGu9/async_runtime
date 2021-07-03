#include <ctime>
#include <algorithm>
#include "async_runtime/fundamental/async.h"
#include "async_runtime/fundamental/timer.h"

class Timer::TimerThread : public Object
{
public:
    struct TimerTask
    {
        std::chrono::system_clock::time_point timePoint;
        Function<void()> task;
    };
    TimerThread();
    virtual ~TimerThread();
    virtual void post(TimerTask task);

protected:
    static const std::greater<TimerTask> greater;
    std::mutex _mutex;
    std::condition_variable _cv;
    std::deque<TimerTask> _tasks;
    Thread _thread;

    bool _stop = false;
    bool _hasNewTask = false;
};

namespace std
{
    template <>
    struct greater<Timer::TimerThread::TimerTask>
    {
        bool operator()(const Timer::TimerThread::TimerTask &lhs, const Timer::TimerThread::TimerTask &rhs) const
        {
            return lhs.timePoint > rhs.timePoint;
        }
    };
};

const std::greater<Timer::TimerThread::TimerTask> Timer::TimerThread::greater = std::greater<Timer::TimerThread::TimerTask>();

Timer::TimerThread::TimerThread::TimerThread()
{
    _thread = Thread([this]
                     {
#ifndef NDEBUG
                         ThreadPool::thisThreadName = "TimerThread";
                         ThreadPool::setThreadName(ThreadPool::thisThreadName);
#endif
                         while (true)
                         {
                             std::unique_lock<std::mutex> lock(_mutex);
                             if (_tasks.empty())
                                 _cv.wait(lock, [this]
                                          { return _hasNewTask; });
                             else
                                 _cv.wait_until(lock, _tasks.front().timePoint, [this]
                                                { return _hasNewTask; });
                             if (_stop)
                                 return;

                             _hasNewTask = false;
                             const auto now = std::chrono::system_clock::now();
                             while (_tasks.front().timePoint <= now)
                             {
                                 std::pop_heap(_tasks.begin(), _tasks.end(), greater);
                                 _tasks.back().task();
                                 _tasks.pop_back();
                                 if (_tasks.empty())
                                     break;
                             }
                         }
                     });
}

Timer::TimerThread::TimerThread::~TimerThread()
{
    {
        std::unique_lock<std::mutex> lock(_mutex);
        _hasNewTask = true; // wake up the thread
        _stop = true;       // mask the flag that let thread exit
    }
    _cv.notify_all();
    _thread.join();
}

void Timer::TimerThread::TimerThread::post(TimerTask task)
{
    {
        std::unique_lock<std::mutex> lock(_mutex);
        _tasks.emplace_back(std::move(task));
        std::push_heap(_tasks.begin(), _tasks.end(), greater);
        _hasNewTask = true;
    }
    _cv.notify_one();
}

static const ref<Timer::TimerThread> &sharedTimerThread()
{
    static const auto singleton = Object::create<Timer::TimerThread>();
    return singleton;
}

Timer::Timer(ref<State<StatefulWidget>> state, const _FactoryOnly &)
    : Dispatcher(state), _clear(false) {}

Timer::Timer(ref<ThreadPool> callbackHandler, const _FactoryOnly &)
    : Dispatcher(callbackHandler), _clear(false) {}

ref<Timer> Timer::delay(ref<State<StatefulWidget>> state, Duration duration, Function<void()> fn)
{
    static const _FactoryOnly lock = _FactoryOnly();
    ref<Timer> timer = Object::create<Timer>(state, lock);
    timer->_setTimeout(duration, fn);
    return timer;
}

ref<Timer> Timer::periodic(ref<State<StatefulWidget>> state, Duration interval, Function<void()> fn)
{
    static const _FactoryOnly lock = _FactoryOnly();
    ref<Timer> timer = Object::create<Timer>(state, lock);
    timer->_setInterval(interval, fn);
    return timer;
}

ref<Timer> Timer::delay(ref<ThreadPool> callbackHandler, Duration duration, Function<void()> fn)
{
    ref<Timer> timer = Object::create<Timer>(callbackHandler, _FactoryOnly());
    timer->_setTimeout(duration, fn);
    return timer;
}

ref<Timer> Timer::periodic(ref<ThreadPool> callbackHandler, Duration interval, Function<void()> fn)
{
    ref<Timer> timer = Object::create<Timer>(callbackHandler, _FactoryOnly());
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
    assert(this->_clear == false && "Timer can't be reused");
    using std::chrono::system_clock;
    system_clock::time_point current = system_clock::now();
    ref<Timer> self = self(); // hold a ref of self inside the Function
    _onCancel = [] {};
    sharedTimerThread()->post(Timer::TimerThread::TimerTask{
        current + delay.toChronoMilliseconds(),
        [this, self, function]
        {
            if (this->_clear)
                return;
            this->microTaskToMainThread(function);
        }});
}

void Timer::_setInterval(Duration interval, Function<void()> function)
{
    assert(this->_clear == false && "Timer can't be reused");
    using std::chrono::system_clock;
    ref<Timer> self = self(); // hold a ref of self inside the Function
    auto next = std::make_shared<system_clock::time_point>(system_clock::now() + interval.toChronoMilliseconds());
    auto task = std::make_shared<std::function<void()>>();
    *task = [this, self, interval, function, next, task] { // container self-ref [task] cause ref loop
        this->microTaskToMainThread([this, self, interval, function, next, task]
                                    {
                                        if (this->_clear)
                                            return;
                                        function();
                                        *next += interval.toChronoMilliseconds();
                                        std::unique_lock<std::mutex> lock(_mutex);
                                        sharedTimerThread()->post(Timer::TimerThread::TimerTask{*next, *task});
                                    });
    };
    _onCancel = [this, task]
    {
        std::unique_lock<std::mutex> lock(_mutex);
        *task = [] {};
    }; // release self-ref
    sharedTimerThread()->post(Timer::TimerThread::TimerTask{*next, *task});
}

void Timer::cancel()
{
    this->_clear = true;
    _onCancel();
}

void Timer::dispose()
{
    this->cancel();
    Dispatcher::dispose();
}
