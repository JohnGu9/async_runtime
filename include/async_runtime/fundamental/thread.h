#pragma once

#include <thread>
#include "../object.h"

using Thread = std::thread;

class ThreadUnit
{
public:
    static thread_local finalref<String>& threadName;
    static void setThreadName(ref<String> name);
};
