#pragma once

#include "../object.h"
#include "container.h"
#include "ref.h"
#include <deque>
#include <limits>
#include <memory>
#include <sstream>
#include <string>

template <>
class ref<String>;

/**
 * @brief
 * String is Object.
 * Nest it into ref<String> or option<String>.
 * ref<String> and option<String> also work with Map and Set like std::string.
 * String is immutable, the c str is fixed since they were born.
 *
 * @example
 * ref<String> string = "This is a non-null String object"; // directly init ref<String> from const char* const
 * option<String> nullableString = "This is a nullable String object, but now it isn't null with value \"this string\""; // directly init option<String> from const char* const
 * option<String> nullString = nullptr; // this is a null String object
 *
 * ref<String> newString = string + nullableString.assertNotNull(); // append two ref<String> to a new ref<String>
 *
 * if (newString == string) { / *** / } // compare two ref<String>
 * if (newString == "This is a non-null String object") { / *** / } // compare with const char* const
 *
 * ref<String> withTrueString = string + true; // append any type that support ostream &operator<<(std::ostream &out, T b)
 * ref<String> withTrueString = string + 1.023;
 *
 */
class String : public virtual Object, protected std::string
{
    _ASYNC_RUNTIME_FRIEND_FAMILY;

    template <typename T>
    friend struct std::hash;

    friend ref<String> operator+(const char c, const ref<String> &string);
    friend ref<String> operator+(const char *const str, const ref<String> &string);
    friend std::ostream &operator<<(std::ostream &os, const ref<String> &str);
    friend std::ostream &operator<<(std::ostream &os, ref<String> &&str);
    friend std::istream &operator>>(std::istream &is, ref<String> &str);
    friend ref<String> getline(std::istream &is);

    String &operator=(const std::string &other)
    {
        std::string::operator=(other);
        return *this;
    }
    String &operator=(std::string &&other)
    {
        std::string::operator=(std::move(other));
        return *this;
    }

    template <class First, class... Rest>
    static void _unwrapPackToCstr(const char *const str, size_t &lastIndex, std::stringstream &ss, const First &first, const Rest &...rest);
    static void _unwrapPackToCstr(const char *const str, size_t &lastIndex, std::stringstream &ss) {}

    template <typename Iterator, class First, class... Rest>
    static void _unwrapPackToIterator(Iterator &lastIndex, const Iterator &end, std::stringstream &ss, const First &first, const Rest &...rest);
    template <typename Iterator>
    static void _unwrapPackToIterator(Iterator &lastIndex, const Iterator &end, std::stringstream &ss) {}

    template <class First, class... Rest>
    void _unwrapPack(size_t &lastIndex, std::stringstream &ss, const First &first, const Rest &...rest);
    void _unwrapPack(size_t &lastIndex, std::stringstream &ss) {}

public:
    using const_iterator = std::string::const_iterator;
    using const_reverse_iterator = std::string::const_reverse_iterator;
    static const std::size_t npos = std::string::npos;
    class View;

    template <typename... Args>
    static ref<String> formatFromString(const char *const str, Args &&...args);

    template <typename Iterator, typename... Args>
    static ref<String> formatFromIterator(const Iterator begin, const Iterator end, Args &&...args);

    String() {}
    String(const char c) : std::string{c} {}
    String(const char *const str) : std::string(str) { assert(str); }
    String(const char *const str, size_t length) : std::string(str, length) { assert(str); }
    String(const std::string &str) : std::string(str) {}
    String(std::string &&str) : std::string(std::move(str)) {}

    // new interface
    virtual bool isEmpty() const;
    virtual bool isNotEmpty() const;
    virtual bool startsWith(ref<String>) const;
    virtual bool endsWith(ref<String>) const;
    virtual const std::string &toStdString() const { return *this; }
    virtual ref<List<ref<String>>> split(ref<String> pattern) const;

    template <typename... Args>
    ref<String> format(Args &&...args);

    // inherited interface
    virtual const char *const c_str() const { return std::string::c_str(); }
    virtual size_t length() const { return std::string::length(); }
    virtual size_t size() const { return std::string::size(); }

    virtual const char &operator[](size_t index) const { return std::string::operator[](index); }
    virtual const_iterator begin() const { return std::string::begin(); }
    virtual const_iterator end() const { return std::string::end(); }
    virtual const_reverse_iterator rbegin() const { return std::string::rbegin(); }
    virtual const_reverse_iterator rend() const { return std::string::rend(); }

    virtual size_t find(ref<String> pattern, size_t start = 0) const;
    virtual size_t find_first_of(ref<String> pattern, size_t start = 0) const;
    virtual size_t find_first_not_of(ref<String> pattern, size_t start = 0) const;
    virtual size_t find_last_of(ref<String> pattern, size_t start = 0) const;
    virtual size_t find_last_not_of(ref<String> pattern, size_t start = 0) const;
    virtual ref<String> substr(size_t begin = 0, size_t length = SIZE_MAX) const;
};

template <>
class ref<String> : public _async_runtime::RefImplement<String>
{
    _ASYNC_RUNTIME_FRIEND_FAMILY;
    friend class String;
    friend ref<String> operator+(const char c, const ref<String> &string);
    friend ref<String> operator+(const char *const str, const ref<String> &string);
    friend std::ostream &operator<<(std::ostream &os, const ref<String> &dt);
    friend std::ostream &operator<<(std::ostream &os, ref<String> &&dt);
    friend std::istream &operator>>(std::istream &os, ref<String> &str);
    friend ref<String> getline(std::istream &is);

    using const_iterator = String::const_iterator;
    using const_reverse_iterator = String::const_reverse_iterator;

protected:
    ref() {}
    ref(const std::shared_ptr<String> &other) : _async_runtime::RefImplement<String>(other) {}

public:
    ref(const ref<String> &other) : _async_runtime::RefImplement<String>(other){};

    ref(const std::string &str) : _async_runtime::RefImplement<String>(std::make_shared<String>(str)) {}
    ref(std::string &&str) : _async_runtime::RefImplement<String>(std::make_shared<String>(std::move(str))) {}
    ref(const char *const str) : _async_runtime::RefImplement<String>(std::make_shared<String>(str)) {}
    ref(const char c) : _async_runtime::RefImplement<String>(std::make_shared<String>(c)) {}

    bool operator==(const ref<String> &other) const;
    bool operator==(const char *const other) const { return *(*this) == other; }
    bool operator==(const std::string &other) const { return *(*this) == other; }
    bool operator==(std::string &&other) const { return *(*this) == std::move(other); }

    bool operator!=(const ref<String> &other) const { return !operator==(other); }
    bool operator!=(const char *const other) const { return !operator==(other); }
    bool operator!=(const std::string &other) const { return !operator==(other); }
    bool operator!=(std::string &&other) const { return !operator==(std::move(other)); }

    ref<String> operator+(const char c) const;
    ref<String> operator+(const char *const str) const;
    ref<String> operator+(const std::string &other) const;
    ref<String> operator+(std::string &&other) const;
    ref<String> operator+(const ref<String> &other) const;
    ref<String> operator+(ref<Object> object) const;

    template <typename T>
    ref<String> operator+(const T &value) const
    {
        std::stringstream ss;
        ss << *(*this)
#ifndef ASYNC_RUNTIME_DISABLE_BOOL_TO_STRING
           << std::boolalpha
#endif
           << value;
        return ss.str();
    }

    const char &operator[](size_t index) const { return (*(this->get()))[index]; }

    const_iterator begin() const
    {
        return (*this)->begin();
    }

    const_iterator end() const
    {
        return (*this)->end();
    }
};

class String::View : public String
{
    ref<String> _parent;

public:
    // TODO: implement this class
};

ref<String> operator+(const char c, const ref<String> &string);
ref<String> operator+(const char *const str, const ref<String> &string);
std::ostream &operator<<(std::ostream &os, const ref<String> &str);
ref<String> getline(std::istream &os);

template <class First, class... Rest>
void String::_unwrapPackToCstr(const char *const str, size_t &lastIndex, std::stringstream &ss, const First &first, const Rest &...rest)
{
    size_t index = lastIndex;
    while (true)
    {
        if (str[index] == '\0')
        {
#ifndef NDEBUG
            std::stringstream debugInfo;
            debugInfo << "String::format arguments overflow when handle \"" << str << "\"" << std::endl;
            std::cout << debugInfo.str();
#endif
            return;
        }
        if (str[index++] == '{')
        {
            if (str[index++] == '}')
            {
                ss << std::string(&(str[lastIndex]), index - lastIndex - 2)
#ifndef ASYNC_RUNTIME_DISABLE_BOOL_TO_STRING
                   << std::boolalpha
#endif
                   << first;
                lastIndex = index;
                _unwrapPackToCstr(str, lastIndex, ss, rest...);
                return;
            }
        }
    }
}

template <typename... Args>
ref<String> String::formatFromString(const char *const str, Args &&...args)
{
    std::stringstream ss;
    size_t lastIndex = 0;
    _unwrapPackToCstr(str, lastIndex, ss, args...);
    if (str[lastIndex] != '\0')
        ss << std::string(&(str[lastIndex]));
    return ss.str();
}

template <typename Iterator, class First, class... Rest>
void String::_unwrapPackToIterator(Iterator &lastIndex, const Iterator &end, std::stringstream &ss, const First &first, const Rest &...rest)
{
    Iterator index = lastIndex;
    while (true)
    {
        if (index == end)
        {
#ifndef NDEBUG
            std::cout << "String::format arguments overflow" << std::endl;
#endif
            return;
        }
        if (*index == '{')
        {
            if (*(++index) == '}')
            {
                ss << std::string(lastIndex, --index)
#ifndef ASYNC_RUNTIME_DISABLE_BOOL_TO_STRING
                   << std::boolalpha
#endif
                   << first;
                lastIndex = index;
                ++ ++lastIndex; // lastIndex = lastIndex + 2;
                _unwrapPackToIterator(lastIndex, end, ss, rest...);
                return;
            }
        }
        else
            ++index;
    }
}

template <typename Iterator, typename... Args>
ref<String> String::formatFromIterator(const Iterator begin, const Iterator end, Args &&...args)
{
    std::stringstream ss;
    Iterator lastIndex = begin;
    _unwrapPackToIterator(lastIndex, end, ss, args...);
    if (lastIndex != end)
        ss << std::string(lastIndex, end);
    return ss.str();
}

template <class First, class... Rest>
void String::_unwrapPack(size_t &lastIndex, std::stringstream &ss, const First &first, const Rest &...rest)
{
    auto index = this->find("{}", lastIndex);
    if (lastIndex >= this->length() || index == String::npos)
    {
#ifndef NDEBUG
        std::stringstream debugInfo;
        debugInfo << "String::format arguments overflow when handle \"" << *this << "\"" << std::endl;
        std::cout << debugInfo.str();
#endif
        return;
    }
    ss << std::string::substr(lastIndex, index - lastIndex)
#ifndef ASYNC_RUNTIME_DISABLE_BOOL_TO_STRING
       << std::boolalpha
#endif
       << first;
    lastIndex = index + 2;
    _unwrapPack(lastIndex, ss, rest...);
}

template <>
inline ref<String> String::format()
{
    return self();
}

template <typename... Args>
ref<String> String::format(Args &&...args)
{
    std::stringstream ss;
    size_t lastIndex = 0;
    _unwrapPack(lastIndex, ss, args...);
    if (lastIndex < this->length())
        ss << std::string::substr(lastIndex, this->length());
    return ss.str();
}

template <>
bool operator==(const option<String> &object0, option<String> object1);
template <>
bool operator!=(const option<String> &object0, option<String> object1);

namespace std
{
    template <>
    struct hash<::ref<String>>
    {
        std::size_t operator()(const ::ref<String> &other) const
        {
            static const auto hs = hash<std::string>();
            return hs(static_cast<std::string>(*other));
        }
    };

    template <>
    struct hash<::option<String>>
    {
        std::size_t operator()(const ::option<String> &other) const
        {
            lateref<String> str;
            if (other.isNotNull(str))
            {
                static const auto hs = hash<::ref<String>>();
                return hs(str);
            }
            else
            {
                static const auto result = hash<std::shared_ptr<String>>()(nullptr);
                return result;
            }
        }
    };

};
