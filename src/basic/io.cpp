#include "async_runtime/object.h"
#include "async_runtime/basic/io.h"

void print(ref<Object> object) { std::cout << object->toString() << std::endl; }

void print(String str) { std::cout << str << std::endl; }

void print(const char *const str) { std::cout << str << std::endl; }