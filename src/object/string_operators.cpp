#include "async_runtime/object/object.h"

bool String::operator<(const ref<String> &other) const
{
    auto len = other->length();
    if (this->length() == len)
    {
        const char *a = this->data(), *b = other->data();
        for (size_t i = 0; i < len; i++)
        {
            if (a[i] == b[i])
                continue;
            return a[i] < b[i];
        }
        return false;
    }
    return this->length() < len;
}

bool String::operator>(const ref<String> &other) const
{
    auto len = other->length();
    if (this->length() == len)
    {
        const char *a = this->data(), *b = other->data();
        for (size_t i = 0; i < len; i++)
        {
            if (a[i] == b[i])
                continue;
            return a[i] > b[i];
        }
        return false;
    }
    return this->length() > len;
}

bool String::operator<(const option<String> &other) const
{
    if_not_null(other) return this->operator<(other);
    end_if() return false;
}

bool String::operator>(const option<String> &other) const
{
    if_not_null(other) return this->operator>(other);
    end_if() return true;
}

bool String::operator==(const option<Object> &other)
{
    if (auto ptr = dynamic_cast<String *>(other.get()))
    {
        if (this->length() != ptr->length())
            return false;
        return std::strncmp(this->data(), ptr->data(), ptr->length()) == 0;
    }
    return false;
}

bool String::operator==(const std::string &other) const
{
    auto len = other.length();
    if (this->length() != len)
        return false;
    return std::strncmp(this->data(), other.data(), len) == 0;
}

bool String::operator==(const char *const other) const
{
    auto start = this->data();
    size_t i = 0, len = this->length();
    for (; i < len; ++i)
    {
        if (start[i] != other[i])
            return false;
    }
    return other[i] == 0;
}

ref<String> operator+(const char c, ref<String> string)
{
    return String::connect(c, string);
}

ref<String> operator+(const char *const str, ref<String> string)
{
    return String::connect(str, string);
}

ref<String> operator+(const std::string &str, ref<String> string)
{
    return String::connect(str, string);
}

ref<String> operator+(std::string &&str, ref<String> string)
{
    return String::connect(str, string);
}

#include <mutex>

const ref<Map<size_t, ref<String>>> String::_staticCache = Map<size_t, ref<String>>::create();

ref<String> String::_cacheLiterals(const char *c, size_t len)
{
    static std::mutex mutex;
    std::lock_guard<std::mutex> guard(mutex);
    auto it = String::_staticCache->findKey((size_t)c);
    if (it != String::_staticCache->end())
        return it.get()->value()->second;
    auto res = Object::create<String::Static>(c, len);
    String::_staticCache->emplace((size_t)c, res);
    return res;
}

ref<String> operator""_String(const char *c)
{
    return String::_cacheLiterals(c, strlen(c));
}

ref<String> operator""_String(const char *c, size_t len)
{
    return String::_cacheLiterals(c, len);
}
