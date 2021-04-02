#include "async_runtime/object.h"
#include "async_runtime/basic/io.h"

void print(option<Object> object)
{
    lateref<Object> nonNull;
    if (object.isNotNull(nonNull))
    {
        std::cout << nonNull->toString() << std::endl;
    }
    else
    {
        std::cout << "NULL reference" << std::endl;
    }
}

void print(ref<String> str) { std::cout << str->toStdString() << std::endl; }

void print(const char *const str) { std::cout << str << std::endl; }