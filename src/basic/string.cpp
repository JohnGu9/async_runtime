#include "async_runtime/basic/string.h"

String::String(const std::shared_ptr<const std::string> other) : _string(other) {}
String::String(const String &other) : _string(other._string) {}

String::String(const char *const str) : _string(std::make_shared<const std::string>(str)) {}
String::String(const char str) : _string(std::make_shared<const std::string>(std::to_string(str))) {}
String::String(std::string &&str) : _string(std::make_shared<const std::string>(std::forward<std::string>(str))) {}

String::String(const std::stringstream &os) : _string(std::make_shared<const std::string>(os.str())) {}
String::String(std::stringstream &&os) : _string(std::make_shared<const std::string>(os.str())) {}

String::~String() {}

String::operator bool() const { return this->_string != nullptr; }

bool String::operator==(const String &other) const
{
    return other._string == this->_string || *(other._string) == *(this->_string);
}

String String::operator+(const char c) const
{
    std::shared_ptr<std::string> ptr = std::make_shared<std::string>();
    *ptr = *(this->_string) + c;
    return String(ptr);
}

String String::operator+(const char *const str) const
{
    std::shared_ptr<std::string> ptr = std::make_shared<std::string>();
    *ptr = *(this->_string) + str;
    return String(ptr);
}

String String::operator+(const String &other) const
{
    std::shared_ptr<std::string> ptr = std::make_shared<std::string>();
    *ptr = *(this->_string) + *(other._string);
    return String(ptr);
}

bool String::isEmpty() const
{
    return this->_string->empty();
}

bool String::isNotEmpty() const
{
    return !this->isEmpty();
}

const std::string &String::getNativeString() const
{
    return *(this->_string);
}

std::ostream &operator<<(std::ostream &os, const String &str)
{
    os << str._string;
    return os;
}

std::istream &operator>>(std::istream &is, String &str)
{
    std::shared_ptr<std::string> ptr = std::make_shared<std::string>();
    is >> *ptr;
    str._string = ptr;
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
    *ptr += *(string._string);
    return String(ptr);
}

String operator+(const char *const str, const String &string)
{
    std::shared_ptr<std::string> ptr = std::make_shared<std::string>(str);
    *ptr += *(string._string);
    return String(ptr);
}
