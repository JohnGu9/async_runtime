#pragma once
#include "../container.h"
#include "../object.h"
#include <cstring>
#include <iostream>
#include <sstream>
#include <string>

class String : public ReversibleIterable<char>, public ConstIndexableMixin<size_t, char>
{
    template <typename T>
    static bool _formatFromStringToStream(std::ostream &os, const char *&start, const char *end, const T &element);
    template <typename Iterator, typename T>
    static bool _formatFromIteratorToStream(std::ostream &os, Iterator &start, const Iterator &end, const T &element);

    friend ref<String> operator""_String(const char *);
    friend ref<String> operator""_String(const char *c, size_t);
    static ref<String> _cacheLiterals(const char *c, size_t);
    static const ref<Map<size_t, ref<String>>> _staticCache;

protected:
    class View;
    class IteratorBasic;
    String() noexcept = default;

public:
    class Static; // c string
    class Native; // std::string

    class StringConstIterator;
    class ReverseConstIterator;

    using Iterable<char>::find;
    static constexpr const size_t npos = std::string::npos;

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

    bool operator<(const ref<String> &other) const;
    bool operator<(const option<String> &other) const;
    bool operator>(const ref<String> &other) const;
    bool operator>(const option<String> &other) const;

    bool operator==(const char *const other) const;
    bool operator==(const std::string &other) const;

    // override from Object
    bool operator==(const option<Object> &other) override;
    void toStringStream(std::ostream &) override;
    ref<String> toString() override;
    size_t hashCode() override;

    // override from Iterable
    bool any(Function<bool(const char &)>) const override;
    bool every(Function<bool(const char &)>) const override;
    void forEach(Function<void(const char &)>) const override;
    ref<ConstIterator<char>> begin() const override;
    ref<ConstIterator<char>> end() const override;
    ref<ConstIterator<char>> rbegin() const override;
    ref<ConstIterator<char>> rend() const override;

    // inherited from ConstIndexableMixin
    const char &operator[](const size_t &index) const override { return this->data()[index]; }

    // inherited from std::string *
    virtual const char *const data() const = 0;
    virtual const char *const c_str() const { return this->data(); } // will be remove in the future
    virtual size_t length() const { return this->size(); }

    // new interface
    virtual std::string toStdString() const;
    virtual bool startsWith(ref<String>) const;
    virtual bool endsWith(ref<String>) const;
    virtual ref<List<ref<String>>> split(ref<String> pattern) const;
    virtual ref<String> trim() const;
    virtual ref<String> trim(ref<String> pattern) const;
    virtual ref<String> toLowerCase() const;
    virtual ref<String> toUpperCase() const;
    virtual size_t find(ref<String> pattern, size_t pos = 0) const;
    virtual size_t findFirstOf(ref<String> pattern, size_t pos = 0) const;
    virtual size_t findFirstNotOf(ref<String> pattern, size_t pos = 0) const;
    virtual size_t findLastOf(ref<String> pattern, size_t pos = npos) const;
    virtual size_t findLastNotOf(ref<String> pattern, size_t pos = npos) const;
    virtual ref<String> substr(size_t begin = 0, size_t length = npos) const;
    virtual ref<String> replace(size_t begin, size_t length, ref<String> other) const;
    virtual ref<String> reverse() const;

    template <typename... Args>
    ref<String> format(Args &&...args) const;

    template <typename T>
    ref<String> operator+(const T &value) const;
    template <typename T>
    ref<String> operator+(T &&value) const;
};
