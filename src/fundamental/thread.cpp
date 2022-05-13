#include "async_runtime/fundamental/thread.h"

#if defined(_WIN32)
// alarm: do not mass up include order below
#include <windows.h>

#include <processthreadsapi.h>
#endif

thread_local ref<String> ThreadUnit::threadName = "";
void ThreadUnit::setThreadName(ref<String> name)
{
#if defined(_WIN32)
    wchar_t *wtext = new wchar_t[name->length() + 1];
    size_t outputSize;
    mbstowcs_s(&outputSize, wtext, name->length() + 1, name->c_str(), name->length());
    SetThreadDescription(GetCurrentThread(), wtext);
    delete[] wtext;
#elif __APPLE__
    pthread_setname_np(name->c_str());
#elif __linux__
    pthread_setname_np(pthread_self(), name->c_str());
#endif
    threadName = name;
}