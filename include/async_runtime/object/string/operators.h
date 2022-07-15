#pragma once
#include "string.h"

ref<String> operator+(const char c, ref<String> string);
ref<String> operator+(const char *const str, ref<String> string);
ref<String> operator+(const std::string &str, ref<String> string);
ref<String> operator+(std::string &&str, ref<String> string);

ref<String> operator""_String(const char *);
ref<String> operator""_String(const char *, size_t);
