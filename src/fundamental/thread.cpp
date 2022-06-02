#include "async_runtime/fundamental/thread.h"

#ifdef _WIN32
// alarm: do not mass up include order below
#include <windows.h>

#include <processthreadsapi.h>
#endif

thread_local static ref<String> _threadName = "";
thread_local finalref<String> &ThreadUnit::threadName = _threadName;

void ThreadUnit::setThreadName(ref<String> name)
{
#ifdef _WIN32
    wchar_t *wtext = new wchar_t[name->length() + 1];
    size_t outputSize;
    mbstowcs_s(&outputSize, wtext, name->length() + 1, name->data(), name->length());
    SetThreadDescription(GetCurrentThread(), wtext);
    delete[] wtext;
#elif __APPLE__
    pthread_setname_np(name->data());
#elif __linux__
    pthread_setname_np(pthread_self(), name->data());
#endif

    _threadName = name;
}
