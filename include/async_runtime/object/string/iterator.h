#pragma once
#include "string.h"

class String::IteratorBasic : public ConstIterator<char>
{
public:
    const char *const data;
    const size_t position;
    IteratorBasic(const char *const data, const size_t position) noexcept : data(std::move(data)), position(std::move(position)) {}

    const char &value() const override { return data[position]; }
    bool operator==(const option<Object> &other) override;
};

class String::StringConstIterator : public String::IteratorBasic
{
    using super = String::IteratorBasic;

public:
    StringConstIterator(const char *const data, const size_t position) noexcept : super(std::move(data), std::move(position)) {}

    ref<ConstIterator<char>> next() const override;
    ref<ConstIterator<char>> operator+(size_t shift) const;
};

class String::ReverseConstIterator : public String::IteratorBasic
{
    using super = String::IteratorBasic;

public:
    ReverseConstIterator(const char *const data, const size_t position) noexcept : super(std::move(data), std::move(position)) {}

    ref<ConstIterator<char>> next() const override;
    ref<ConstIterator<char>> operator+(size_t shift) const;
};
