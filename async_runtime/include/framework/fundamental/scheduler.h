#pragma once

#include "thread_pool.h"

class Scheduler : public virtual ThreadPool
{
public:
    Scheduler() : ThreadPool(1) {}
};
