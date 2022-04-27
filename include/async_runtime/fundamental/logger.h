#pragma once

#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <time.h>

#include "async.h"

class LoggerHandler : public virtual Object
{
public:
    LoggerHandler() {}

    virtual ref<Future<bool>> write(ref<String> str) = 0;
    virtual ref<Future<bool>> writeLine(ref<String> str) = 0;
    virtual void dispose() = 0;
};
