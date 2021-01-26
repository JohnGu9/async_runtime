#include "async_runtime/basic/string.h"

String::String(const std::shared_ptr<const std::string> other) : _ptr(other) {}
String::String(const String &other) : _ptr(other._ptr) {}

String::String(const char *const str) : _ptr(std::make_shared<const std::string>(str)) {}
String::String(const char str) : _ptr(std::make_shared<const std::string>(std::to_string(str))) {}
String::String(std::string &&str) : _ptr(std::make_shared<const std::string>(std::forward<std::string>(str))) {}

String::String(const std::stringstream &os) : _ptr(std::make_shared<const std::string>(os.str())) {}
String::String(std::stringstream &&os) : _ptr(std::make_shared<const std::string>(os.str())) {}

String::~String() {}

String::operator bool() const { return this->_ptr != nullptr; }

bool String::operator==(const String &other) const
{
    return other._ptr == this->_ptr || *(other._ptr) == *(this->_ptr);
}

bool String::operator==(const char *const other) const
{
    return *(this->_ptr) == other;
}

String String::operator+(const char c) const
{
    std::shared_ptr<std::string> ptr = std::make_shared<std::string>();
    *ptr = *(this->_ptr) + c;
    return String(ptr);
}

String String::operator+(const char *const str) const
{
    std::shared_ptr<std::string> ptr = std::make_shared<std::string>();
    *ptr = *(this->_ptr) + str;
    return String(ptr);
}

String String::operator+(const String &other) const
{
    std::shared_ptr<std::string> ptr = std::make_shared<std::string>();
    *ptr = *(this->_ptr) + *(other._ptr);
    return String(ptr);
}

bool String::isEmpty() const
{
    return this->_ptr->empty();
}

bool String::isNotEmpty() const
{
    return !this->isEmpty();
}

const std::string &String::getNativeString() const
{
    return *(this->_ptr);
}

std::ostream &operator<<(std::ostream &os, const String &str)
{
    os << *(str._ptr);
    return os;
}

std::istream &operator>>(std::istream &is, String &str)
{
    std::shared_ptr<std::string> ptr = std::make_shared<std::string>();
    is >> *ptr;
    str._ptr = ptr;
    return is;
}

String getline(std::istream &is)
{
    std::shared_ptr<std::string> ptr = std::make_shared<std::string>();
    getline(is, *ptr);
    return String(ptr);
}

String operator+(const char c, const String &string)
{
    std::shared_ptr<std::string> ptr = std::make_shared<std::string>(std::to_string(c));
    *ptr += *(string._ptr);
    return String(ptr);
}

String operator+(const char *const str, const String &string)
{
    std::shared_ptr<std::string> ptr = std::make_shared<std::string>(str);
    *ptr += *(string._ptr);
    return String(ptr);
}
