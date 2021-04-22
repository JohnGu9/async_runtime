#include <ctime>
#include <algorithm>
#include "async_runtime/fundamental/async.h"
#include "async_runtime/fundamental/timer.h"

struct TimerTask
{
    std::chrono::system_clock::time_point timePoint;
    Function<void()> task;
};

namespace std
{
    template <>
    struct greater<TimerTask>
    {
        bool operator()(const TimerTask &lhs, const TimerTask &rhs) const
        {
            return lhs.timePoint > rhs.timePoint;
        }
    };
};

class TimerThread : public Object
{
    static const std::greater<TimerTask> greater;

public:
    TimerThread()
    {
        _thread = Thread([this] {
#ifdef DEBUG
            ThreadPool::thisThreadName = "TimerThread";
            ThreadPool::setThreadName(ThreadPool::thisThreadName);
#endif
            while (true)
            {
                std::unique_lock<std::mutex> lock(_mutex);
                if (_timePoints.empty())
                    _cv.wait(lock, [this] { return _hasNewTask || _stop; });
                else
                    _cv.wait_until(lock, _timePoints.front().timePoint, [this] { return _hasNewTask || _stop; });
                if (_stop)
                    return;

                _hasNewTask = false;
                const auto now = std::chrono::system_clock::now();
                while (_timePoints.front().timePoint <= now)
                {
                    std::pop_heap(_timePoints.begin(), _timePoints.end(), greater);
                    _timePoints.back().task();
                    _timePoints.pop_back();
                    if (_timePoints.empty())
                        break;
                }
            }
        });
    }

    virtual ~TimerThread()
    {
        {
            std::unique_lock<std::mutex> lock(_mutex);
            _stop = true;
        }
        _cv.notify_all();
        _thread.join();
    }

    virtual void post(TimerTask task)
    {
        {
            std::unique_lock<std::mutex> lock(_mutex);
            _timePoints.emplace_back(std::move(task));
            std::push_heap(_timePoints.begin(), _timePoints.end(), greater);
            _hasNewTask = true;
        }
        _cv.notify_one();
    }

protected:
    std::mutex _mutex;
    std::condition_variable _cv;
    std::deque<TimerTask> _timePoints;
    Thread _thread;

    bool _stop = false;
    bool _hasNewTask = false;
};

const std::greater<TimerTask> TimerThread::greater = std::greater<TimerTask>();

static const ref<TimerThread> &sharedTimerThread()
{
    static const auto singleton = Object::create<TimerThread>();
    return singleton;
}

Timer::Timer(State<StatefulWidget> *state, const _FactoryOnly &)
    : Dispatcher(state), _clear(false) {}

Timer::Timer(ref<ThreadPool> callbackHandler, const _FactoryOnly &)
    : Dispatcher(callbackHandler), _clear(false) {}

ref<Timer> Timer::delay(State<StatefulWidget> *state, Duration duration, Function<void()> fn)
{
    static const _FactoryOnly lock = _FactoryOnly();
    ref<Timer> timer = Object::create<Timer>(state, lock);
    timer->_setTimeout(duration, fn);
    return timer;
}

ref<Timer> Timer::periodic(State<StatefulWidget> *state, Duration interval, Function<void()> fn)
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
    sharedTimerThread()->post(TimerTask{
        current + delay.toChronoMilliseconds(),
        [self, function] {
            if (self->_clear)
                return;
            self->microTask(function);
        }});
}

void Timer::_setInterval(Duration interval, Function<void()> function)
{
    assert(this->_clear == false && "Timer can't be reused");
    using std::chrono::system_clock;
    auto next = std::make_shared<system_clock::time_point>(system_clock::now() + interval.toChronoMilliseconds());
    ref<Timer> self = self(); // hold a ref of self inside the Function
    auto task = std::make_shared<std::function<void()>>();
    *task = [=] { // container self-ref [task] cause ref loop
        self->microTask([=] {
            if (self->_clear)
                return;
            function();
            *next += interval.toChronoMilliseconds();
            std::unique_lock<std::mutex> lock(_mutex);
            sharedTimerThread()->post(TimerTask{*next, *task});
        });
    };
    _onCancel = [this, task] {
        std::unique_lock<std::mutex> lock(_mutex);
        *task = [] {};
    }; // release self-ref
    sharedTimerThread()->post(TimerTask{*next, *task});
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
