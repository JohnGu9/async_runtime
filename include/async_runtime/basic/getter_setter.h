#pragma once

template <typename T>
class getter
{
public:
    virtual const T &getValue() const = 0;
    virtual T &getValue() = 0;
};

template <typename T>
class setter
{
public:
    virtual void setValue(const T &) = 0;
    virtual void setValue(T &&) = 0;
};