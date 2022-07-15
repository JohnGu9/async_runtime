#pragma once
#include "string.h"

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
    if (result[sizeof(result) - 1] == false) // [[unlikely]]
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
            auto next = start;
            ++next;
            if (next != end && *next == '}')
            {
                os << element;
                start = ++next;
                return true;
            }
        }
        os << *start;
        ++start;
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
    if (result[sizeof(result) - 1] == false) // [[unlikely]]
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
inline ref<String> String::format() const { return constSelf(); }

template <typename... Args>
ref<String> String::format(Args &&...args) const
{
    std::stringstream ss;
#ifndef ASYNC_RUNTIME_DISABLE_BOOL_TO_STRING
    ss << std::boolalpha;
#endif
    String::formatFromStringToStream<>(ss, this->data(), this->length(), std::forward<Args>(args)...);
    return ss.str();
}
