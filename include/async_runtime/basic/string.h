#pragma once
#include <string>
#include <sstream>
#include <memory>

class String
{
    friend String operator+(const char c, const String &string);
    friend String operator+(const char *const str, const String &string);
    friend std::ostream &operator<<(std::ostream &os, const String &dt);
    friend std::istream &operator>>(std::istream &os, String &str);
    friend String getline(std::istream &is);
    friend class std::hash<String>;

    std::shared_ptr<const std::string> _ptr;
    String(const std::shared_ptr<const std::string> other);

public:
    String() {}
    String(const String &other);
    String(const char *const str);
    String(const char str);
    String(std::string &&str);
    String(const std::stringstream &os);
    String(std::stringstream &&os);
    virtual ~String();

    virtual operator bool() const;
    virtual bool operator==(const String &other) const;
    virtual String operator+(const char c) const;
    virtual String operator+(const char *const str) const;
    virtual String operator+(const String &other) const;

    virtual bool isEmpty() const;
    virtual bool isNotEmpty() const;
    virtual const std::string &getNativeString() const;
};

String operator+(const char c, const String &string);
String operator+(const char *const str, const String &string);
std::ostream &operator<<(std::ostream &os, const String &str);
std::istream &operator>>(std::istream &is, String &str);
String getline(std::istream &os);

namespace std
{
    template <>
    struct hash<String>
    {
        std::size_t operator()(const String &str) const
        {
            return hash<std::string>()(*(str._ptr));
        }
    };
}