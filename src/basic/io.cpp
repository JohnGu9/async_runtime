#include "async_runtime/object.h"

void print(Object::Ref<Object> object) { std::cout << object->toString() << std::endl; }

void print(std::string &str) { std::cout << str << std::endl; }

void print(const char *const str) { std::cout << str << std::endl; }