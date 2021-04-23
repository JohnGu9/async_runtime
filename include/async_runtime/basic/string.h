/// [String] is [Object] now
/// Do not use [String] directly
/// Wrap [String] with nullsafety api like [ref] or [option]
/// Use ref<String> or option<String>
/// And it will behave like a nullsafety object and with some extra feature
///
///
/// example:
///
/// ref<String> string = "This is a non-null String object"; // directly init ref<String> from const char* const
///
/// option<String> nullableString = "This is a nullable String object, but now it isn't null with value \"this string\""; // directly init option<String> from const char* const
/// option<String> nullString = nullptr; // this is a null String object
///
/// ref<String> newString = string + nullableString.assertNotNull(); // append two ref<String> to a new ref<String>
///
/// if (newString == string) { / *** / } // compare two ref<String>
/// else { / *** / }
///
/// ref<String> withTrueString = string + true; // append any type that support ostream &operator<<(std::ostream &out, T b)
/// ref<String> withTrueString = string + 1.023;
///
/// // ref<String> and option<String> also work with Map and Set like std::string
///

#pragma once

#include <string>
#include <sstream>
#include <memory>
#include <limits>
#include <deque>
#include "ref.h"
#include "../object.h"

template <>
class option<String>;
template <>
class ref<String>;

class String : public Object, protected std::string
{
    template <typename T>
    friend class ref;

    template <typename T>
    friend struct std::hash;

    friend class option<String>;

    friend ref<String> operator+(const char c, const ref<String> &string);
    friend ref<String> operator+(const char *const str, const ref<String> &string);
    friend std::ostream &operator<<(std::ostream &os, const ref<String> &str);
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
    void _unwrapPack(std::stringstream &ss, size_t &lastIndex, const First &first, const Rest &...rest);

    void _unwrapPack(std::stringstream &ss, size_t &lastIndex) {}

public:
    using const_iterator = std::string::const_iterator;
    using const_reverse_iterator = std::string::const_reverse_iterator;
    class View;

    String() {}
    String(const char str) : std::string{str} {}
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
    virtual size_t find_first_of(ref<String> pattern) const;
    virtual size_t find_first_not_of(ref<String> pattern) const;
    virtual size_t find_last_of(ref<String> pattern) const;
    virtual size_t find_last_not_of(ref<String> pattern) const;
    virtual ref<String> substr(size_t begin = 0, size_t length = SIZE_MAX) const;
};

template <>
class option<String> : protected std::shared_ptr<String>, public ar::ToRefMixin<String>
{
    template <typename R>
    friend class weakref;

    template <typename R>
    friend class ar::RefImplement;

    template <typename R>
    friend class ref;

    template <typename R>
    friend class lateref;

    friend class Object;

public:
    static option<String> null()
    {
        static const option<String> instance = nullptr;
        return instance;
    }

    option() {}
    option(std::nullptr_t) : std::shared_ptr<String>(nullptr) {}
    option(const char *const str) : std::shared_ptr<String>(std::make_shared<String>(str)) {}
    option(const std::string &str) : std::shared_ptr<String>(std::make_shared<String>(str)) {}
    option(std::string &&str) : std::shared_ptr<String>(std::make_shared<String>(std::move(str))) {}

    template <typename R, typename std::enable_if<std::is_base_of<String, R>::value>::type * = nullptr>
    option(const ref<R> &other);

    template <typename R, typename std::enable_if<std::is_base_of<String, R>::value>::type * = nullptr>
    option(const option<R> &other) : std::shared_ptr<String>(std::static_pointer_cast<String>(other)) {}

    bool isNotNull(ref<String> &) const override;
    ref<String> isNotNullElse(std::function<ref<String>()>) const override;
    ref<String> assertNotNull() const override;
    bool operator==(const option<String> &other) const;
    bool operator!=(const option<String> &other) const;

    String *operator->() const = delete;
    operator bool() const = delete;

protected:
    template <typename R, typename std::enable_if<std::is_base_of<String, R>::value>::type * = nullptr>
    option(const std::shared_ptr<R> &other) : std::shared_ptr<String>(std::static_pointer_cast<String>(other)){};
};

template <>
class ref<String> : public ar::RefImplement<String>
{
protected:
    template <typename R>
    friend class option;

    template <typename R>
    friend class weakref;

    template <typename R>
    friend class Future;

    friend class String;

    friend ref<String> operator+(const char c, const ref<String> &string);
    friend ref<String> operator+(const char *const str, const ref<String> &string);
    friend std::ostream &operator<<(std::ostream &os, const ref<String> &dt);
    friend std::istream &operator>>(std::istream &os, ref<String> &str);
    friend ref<String> getline(std::istream &is);

    ref() {}
    ref(const std::shared_ptr<String> &other) : ar::RefImplement<String>(other) {}

public:
    ref(const ref<String> &other) : ar::RefImplement<String>(other){};
    ref(const std::string &str) : ar::RefImplement<String>(std::make_shared<String>(str)) {}
    ref(std::string &&str) : ar::RefImplement<String>(std::make_shared<String>(std::move(str))) {}
    ref(const char *const str) : ar::RefImplement<String>(std::make_shared<String>(str)) {}
    ref(const char str) : ar::RefImplement<String>(std::make_shared<String>(str)) {}

    bool operator==(const ref<String> &other) const
    {
        if (this->get() == other.get())
            return true;
        if ((*this)->length() != other->length())
            return false;
        return std::equal((*this)->begin(), (*this)->end(), other->begin());
    }
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

    const char &operator[](size_t index) { return (*(this->get()))[index]; }
};

class String::View : public String
{
    ref<String> _parent;

public:
};

ref<String> operator+(const char c, const ref<String> &string);
ref<String> operator+(const char *const str, const ref<String> &string);
std::ostream &operator<<(std::ostream &os, const ref<String> &str);
std::istream &operator>>(std::istream &is, ref<String> &str);
ref<String> getline(std::istream &os);
void print(ref<String> str);

template <typename R, typename std::enable_if<std::is_base_of<String, R>::value>::type *>
option<String>::option(const ref<R> &other) : std::shared_ptr<String>(static_cast<std::shared_ptr<R>>(other)) {}

template <class First, class... Rest>
void String::_unwrapPack(std::stringstream &ss, size_t &lastIndex, const First &first, const Rest &...rest)
{
    auto index = this->find("{}", lastIndex);
    if (lastIndex >= this->length() || index == std::string::npos)
    {
#ifdef DEBUG
        std::stringstream ss;
        ss << "String::format arguments overflow when handle \"" << *this << "\"" << std::endl;
        std::cout << ss.str();
#endif
        return;
    }
    ss << std::string::substr(lastIndex, index - lastIndex)
#ifndef ASYNC_RUNTIME_DISABLE_BOOL_TO_STRING
       << std::boolalpha
#endif
       << first;
    lastIndex = index + 2;
    _unwrapPack(ss, lastIndex, rest...);
}

template <typename... Args>
ref<String> String::format(Args &&...args)
{
    std::stringstream ss;
    size_t lastIndex = 0;
    _unwrapPack(ss, lastIndex, args...);
    if (lastIndex < this->length())
        ss << std::string::substr(lastIndex, this->length());
    return ss.str();
}

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