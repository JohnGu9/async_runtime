#include "async_runtime/fundamental/async_thread_pool.h"

AsyncThreadPool::AsyncThreadPool(bool autoDispose) : _autoDispose(autoDispose) {}

AsyncThreadPool::~AsyncThreadPool() {}

void AsyncThreadPool::launch(Function<void()> fn)
{
    {
        std::unique_lock<std::mutex> lock(_mutex);
        if (_stop) // [[unlikely]]
            throw std::runtime_error("Async task post after ThreadPool disposed. The task will be dropped and future will never return. ");

        _tasks.emplace(fn);
    }
    _condition.notify_one();
}

bool AsyncThreadPool::isActive()
{
    std::unique_lock<std::mutex> lock(_mutex);
    return !this->_stop;
}

AsyncThreadPool::Single::Single(bool autoDispose)
    : super(autoDispose),
      _worker([this] { //
          while (true)
          {
              lateref<Fn<void()>> task;
              {
                  std::unique_lock<std::mutex> lock(this->_mutex);
                  this->_condition.wait(lock, [this]
                                        { return this->_stop || !this->_tasks.empty(); });
                  if (this->_dropRemainTasks) // [[unlikely]]
                  {
                      return;
                  }
                  else if (!this->_tasks.empty()) // [[likely]]
                  {
                      task = std::move(this->_tasks.front());
                      this->_tasks.pop();
                  }
                  else if (this->_stop) // [[unlikely]]
                  {
                      return;
                  }
                  else // [[unlikely]]
                  {
                      continue;
                  }
              }
              task();
          }
      })
{
}

void AsyncThreadPool::Single::dispose(bool dropRemainTasks)
{
    {
        std::unique_lock<std::mutex> lock(_mutex);
        if (_stop)
        {
            return;
        }
        _dropRemainTasks = false;
        _stop = true;
    }
    _condition.notify_one();
    _worker.join();
}

AsyncThreadPool::Single::~Single()
{
    if (_autoDispose)
    {
        Single::dispose();
    }
    else
    {
        assert(_stop);
    }
}

class AsyncThreadPool::Multi : public AsyncThreadPool
{
    using super = AsyncThreadPool;
    bool _dropRemainTasks = false;

public:
    std::vector<Thread> _workers;

    Multi(size_t threads, bool autoDispose) : super(autoDispose)
    {
        for (size_t i = 0; i < threads; ++i)
            _workers.emplace_back([this] { //
                while (true)
                {
                    lateref<Fn<void()>> task;
                    {
                        std::unique_lock<std::mutex> lock(this->_mutex);
                        this->_condition.wait(lock, [this]
                                              { return this->_stop || !this->_tasks.empty(); });

                        if (this->_dropRemainTasks) // [[unlikely]]
                        {
                            return;
                        }
                        else if (!this->_tasks.empty()) // [[likely]]
                        {
                            task = std::move(this->_tasks.front());
                            this->_tasks.pop();
                        }
                        else if (this->_stop) // [[unlikely]]
                        {
                            return;
                        }
                        else // [[unlikely]]
                        {
                            continue;
                        }
                    }
                    task();
                }
            });
    }

    void dispose(bool dropRemainTasks = false) override
    {
        {
            std::unique_lock<std::mutex> lock(_mutex);
            if (_stop)
            {
                return;
            }
            _dropRemainTasks = false;
            _stop = true;
        }
        _condition.notify_all();

        for (std::thread &worker : _workers)
            worker.join();
        _workers.clear();
    }

    ~Multi()
    {
        if (_autoDispose)
        {
            Multi::dispose();
        }
        else
        {
            assert(_stop);
        }
    }
};

ref<AsyncThreadPool::Single> AsyncThreadPool::single(bool autoDispose)
{
    return Object::create<AsyncThreadPool::Single>(autoDispose);
}

ref<AsyncThreadPool> AsyncThreadPool::fromThreadCount(size_t n, bool autoDispose)
{
    assert(n != 0);
    if (n == 1)
    {
        return AsyncThreadPool::single(autoDispose);
    }
    return Object::create<AsyncThreadPool::Multi>(n, autoDispose);
}
