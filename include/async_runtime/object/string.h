#pragma once

#include "container.h"
#include "object.h"
#include <cstring>
#include <iostream>
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
 * ref<String> withNumberString = string + 1.023;
 *
 */
class String : public Iterable<char>, protected std::string
{
    _ASYNC_RUNTIME_FRIEND_FAMILY;

    template <typename T>
    friend struct std::hash;

    template <typename T>
    static bool _formatFromStringToStream(std::ostream &os, const char *&start, const char *end, const T &element);

    template <typename Iterator, typename T>
    static bool _formatFromIteratorToStream(std::ostream &os, Iterator &start, const Iterator &end, const T &element);

    using super = std::string;

protected:
    class View;
    String() {}
    virtual bool isNative() { return true; }

public:
    using iterator = super::const_iterator;
    using reverse_iterator = super::const_reverse_iterator;
    using const_iterator = super::const_iterator;
    using const_reverse_iterator = super::const_reverse_iterator;

    static const size_t npos = super::npos;

    static ref<String> getline(std::istream &os);

    template <typename... Args>
    static void connectToStream(std::ostream &ss, Args &&...args);
    template <typename... Args>
    static ref<String> connect(Args &&...args);

    template <typename... Args>
    static void formatFromStringToStream(std::ostream &os, const char *const str, size_t len, Args &&...args);
    template <typename... Args>
    static ref<String> formatFromString(const char *const str, Args &&...args);

    template <typename Iterator, typename... Args>
    static void formatFromIteratorToStream(std::ostream &os, const Iterator begin, const Iterator end, Args &&...args);
    template <typename Iterator, typename... Args>
    static ref<String> formatFromIterator(const Iterator begin, const Iterator end, Args &&...args);

    String(const char c) : super{c} {}
    String(const std::string &str) : super(str) {}
    String(std::string &&str) : super(std::move(str)) {}

    String(const char *const str) : super(str) { DEBUG_ASSERT(str != nullptr); }
    String(const char *const str, size_t length) : super(str, length) { DEBUG_ASSERT(str != nullptr); }

    String &operator=(const char other) = delete;
    String &operator=(const char *const other) = delete;
    String &operator=(const std::string &other) = delete;
    String &operator=(std::string &&other) = delete;

    size_t find_first_of(const char *_Ptr, size_t _Off, size_t _Count) const = delete;
    size_t find_first_not_of(const char *_Ptr, size_t _Off, size_t _Count) const = delete;
    size_t find_last_of(const char *_Ptr, size_t _Off, size_t _Count) const = delete;
    size_t find_last_not_of(const char *_Ptr, size_t _Off, size_t _Count) const = delete;

    virtual bool operator==(const ref<String> &other);

    // override from Object
    bool operator==(ref<Object> other) override;
    void toStringStream(std::ostream &) override;
    ref<String> toString() override;

    // override from Iterable
    bool any(Function<bool(const char &)>) const override;
    bool every(Function<bool(const char &)>) const override;
    void forEach(Function<void(const char &)>) const override;

    // new interface
    virtual std::string toStdString() const;
    virtual bool isEmpty() const;
    virtual bool isNotEmpty() const;
    virtual bool startsWith(ref<String>) const;
    virtual bool endsWith(ref<String>) const;
    virtual ref<List<ref<String>>> split(ref<String> pattern) const;
    virtual ref<String> trim() const;
    virtual ref<String> trim(ref<String> pattern) const;
    virtual ref<String> toLowerCase() const;
    virtual ref<String> toUpperCase() const;

    template <typename... Args>
    ref<String> format(Args &&...args);

    virtual size_t find(ref<String> pattern, size_t start = 0) const;
    virtual size_t findFirstOf(ref<String> pattern, size_t start = 0) const;
    virtual size_t findFirstNotOf(ref<String> pattern, size_t start = 0) const;
    virtual size_t findLastOf(ref<String> pattern, size_t start = npos) const;
    virtual size_t findLastNotOf(ref<String> pattern, size_t start = npos) const;
    virtual ref<String> substr(size_t begin = 0, size_t length = npos) const;

    // inherited interface
    virtual const char *const c_str() const { return super::c_str(); } // will be remove in the future
    virtual size_t size() const { return super::size(); }              // will be remove in the future

    virtual const char *const data() const { return super::data(); }
    virtual size_t length() const { return super::length(); }

    virtual const char &operator[](size_t index) const { return super::operator[](index); }
    virtual const_iterator begin() const { return super::begin(); }
    virtual const_iterator end() const { return super::end(); }
    virtual const_reverse_iterator rbegin() const { return super::rbegin(); }
    virtual const_reverse_iterator rend() const { return super::rend(); }
};

template <>
class ref<String> : public _async_runtime::RefImplement<String>
{
    _ASYNC_RUNTIME_FRIEND_FAMILY;
    friend class String;

    using const_iterator = String::const_iterator;
    using const_reverse_iterator = String::const_reverse_iterator;
    using super = _async_runtime::RefImplement<String>;

protected:
    ref() {}
    ref(const std::shared_ptr<String> &other) : super(other) {}

public:
    template <typename R, typename std::enable_if<std::is_base_of<String, R>::value>::type * = nullptr>
    ref(const ref<R> &other) : super(other) {}

    ref(const std::string &str) : super(std::make_shared<String>(str)) {}
    ref(std::string &&str) : super(std::make_shared<String>(std::move(str))) {}
    ref(const char *const str) : super(std::make_shared<String>(str)) {}
    ref(const char c) : super(std::make_shared<String>(c)) {}

    bool operator==(const char *const other) const;
    bool operator==(const std::string &other) const;
    bool operator==(std::string &&other) const;

    bool operator!=(const char *const other) const { return !operator==(other); }
    bool operator!=(const std::string &other) const { return !operator==(other); }
    bool operator!=(std::string &&other) const { return !operator==(std::move(other)); }

    const char &operator[](size_t index) const { return this->get()->operator[](index); }
    const_iterator begin() const { return (*this)->begin(); }
    const_iterator end() const { return (*this)->end(); }

    template <typename T>
    ref<String> operator+(const T &value) const { return String::connect(*this, value); }
};

template <typename... Args>
void String::connectToStream(std::ostream &os, Args &&...args)
{
    using expander = bool[]; // on Windows platform, must use redefine
    (void)expander{true, (void(os << args), true)...};
}

template <typename... Args>
ref<String> String::connect(Args &&...args)
{
    std::stringstream ss;
#ifndef ASYNC_RUNTIME_DISABLE_BOOL_TO_STRING
    ss << std::boolalpha;
#endif
    String::connectToStream<>(ss, std::forward<Args>(args)...);
    return ss.str();
}

template <typename T>
bool String::_formatFromStringToStream(std::ostream &os, const char *&start, const char *end, const T &element)
{
    const auto originStart = start;
    const auto endPosition = end - 1;
    for (; start < endPosition; start++)
    {
        if (std::strncmp(start, "{}", 2) == 0)
        {
            os.write(originStart, start - originStart);
            os << element;
            start += 2;
            return true;
        }
    }
    // [[unlikely]]
    os.write(originStart, start - originStart);
    return false;
}

template <typename... Args>
void String::formatFromStringToStream(std::ostream &os, const char *const str, size_t len, Args &&...args)
{
    const char *start = str;
    const char *end = start + len;

    using expander = bool[]; // on Windows platform, must use redefine
#if !defined(NDEBUG) && !defined(ASYNC_RUNTIME_STRING_NO_WARNING)
    bool result[] = {true, String::_formatFromStringToStream<>(os, start, end, args)...};
    if (result[sizeof(result) - 1] == false) //[[unlikely]]
    {
        std::cout << "Warning: String format \"" << str << "\" with arguments(" << sizeof...(args) << ") ";
        (void)expander{true, (void(std::cout << '[' << args << "] "), true)...};
        std::cout << "overflow" << std::endl;
    }
#else
    (void)expander{true, String::_formatFromStringToStream<>(os, start, end, args)...};
#endif

    os.write(start, end - start);
}

template <typename... Args>
ref<String> String::formatFromString(const char *const str, Args &&...args)
{
    std::stringstream ss;
#ifndef ASYNC_RUNTIME_DISABLE_BOOL_TO_STRING
    ss << std::boolalpha;
#endif
    auto len = std::strlen(str);
    String::formatFromStringToStream<>(ss, str, len, std::forward<Args>(args)...);
    return ss.str();
}

template <typename Iterator, typename T>
bool String::_formatFromIteratorToStream(std::ostream &os, Iterator &start, const Iterator &end, const T &element)
{
    while (start != end)
    {
        if (*start == '{')
        {
            auto next = start + 1;
            if (next != end && *next == '}')
            {
                os << element;
                start = next + 1;
                return true;
            }
        }
        os << *start;
        start++;
    }
    // [[unlikely]]
    return false;
}

template <typename Iterator, typename... Args>
void String::formatFromIteratorToStream(std::ostream &os, const Iterator begin, const Iterator end, Args &&...args)
{
    Iterator start = begin;
    using expander = bool[]; // on Windows platform, must use redefine
#if !defined(NDEBUG) && !defined(ASYNC_RUNTIME_STRING_NO_WARNING)
    bool result[] = {true, String::_formatFromIteratorToStream<>(os, start, end, args)...};
    if (result[sizeof(result) - 1] == false) //[[unlikely]]
    {
        std::cout << "Warning: String format \"" << std::string(begin, end) << "\" with arguments(" << sizeof...(args) << ") ";
        (void)expander{true, (void(std::cout << '[' << args << "] "), true)...};
        std::cout << "overflow" << std::endl;
    }
#else
    (void)expander{true, String::_formatFromIteratorToStream<>(os, start, end, args)...};
#endif
    for (; start != end; start++)
        os << *start;
}

template <typename Iterator, typename... Args>
ref<String> String::formatFromIterator(const Iterator begin, const Iterator end, Args &&...args)
{
    std::stringstream ss;
#ifndef ASYNC_RUNTIME_DISABLE_BOOL_TO_STRING
    ss << std::boolalpha;
#endif
    String::formatFromIteratorToStream<>(ss, begin, end, std::forward<Args>(args)...);
    return ss.str();
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
#ifndef ASYNC_RUNTIME_DISABLE_BOOL_TO_STRING
    ss << std::boolalpha;
#endif
    String::formatFromStringToStream<>(ss, this->data(), this->length(), std::forward<Args>(args)...);
    return ss.str();
}

ref<String> operator+(const char c, ref<String> string);
ref<String> operator+(const char *const str, ref<String> string);
ref<String> operator+(const std::string &str, ref<String> string);
ref<String> operator+(std::string &&str, ref<String> string);

std::ostream &operator<<(std::ostream &os, const ref<String> &str);
std::ostream &operator<<(std::ostream &os, ref<String> &&str);
