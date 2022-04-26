#pragma once

#include <thread>
#include "../object.h"

using Thread = std::thread;

class ThreadUnit
{
public:
    static thread_local ref<String> threadName;
    static void setThreadName(ref<String> name);
};
