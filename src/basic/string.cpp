#include "async_runtime/basic/string.h"

bool option<String>::isNotNull(ref<String> &object) const
{
    const std::shared_ptr<String> ptr = static_cast<const std::shared_ptr<String>>(*this);
    if (ptr != nullptr)
    {
        object = ptr;
        return true;
    }
    else
    {
        return false;
    }
}

bool option<String>::operator==(const option<String> &other) const
{
    if (static_cast<const std::shared_ptr<String>>(*this) == nullptr)
    {
        if (static_cast<const std::shared_ptr<String>>(other) == nullptr)
            return true;
        else
            return false;
    }
    else
    {
        if (static_cast<const std::shared_ptr<String>>(other) == nullptr)
            return true;
        else
            return this->get() == other.get();
    }
}

bool option<String>::operator!=(const option<String> &other) const
{
    return this->operator==(other);
}

ref<String> option<String>::isNotNullElse(std::function<ref<String>()> fn) const
{
    const std::shared_ptr<String> ptr = static_cast<const std::shared_ptr<String>>(*this);
    if (ptr != nullptr)
    {
        return ref<String>(ptr);
    }
    else
    {
        return fn();
    }
}

ref<String> option<String>::assertNotNull() const
{
    const std::shared_ptr<String> ptr = static_cast<const std::shared_ptr<String>>(*this);
    if (ptr != nullptr)
    {
        return ref<String>(ptr);
    }
    else
    {
        throw std::runtime_error(std::string(typeid(*this).name()) + " assert not null on a null ref. ");
    }
}

ref<String> ref<String>::operator+(const char c) const
{
    std::shared_ptr<String> ptr = std::make_shared<String>();
    *ptr = *(*this) + c;
    return ref<String>(ptr);
}

ref<String> ref<String>::operator+(const char *const str) const
{
    std::shared_ptr<String> ptr = std::make_shared<String>();
    *ptr = *(*this) + str;
    return ref<String>(ptr);
}

ref<String> ref<String>::operator+(const std::string &other) const
{
    std::shared_ptr<String> ptr = std::make_shared<String>();
    *ptr = *(*this) + other;
    return ref<String>(ptr);
}

ref<String> ref<String>::operator+(std::string &&other) const
{
    std::shared_ptr<String> ptr = std::make_shared<String>();
    *ptr = *(*this) + other;
    return ref<String>(ptr);
}

ref<String> ref<String>::operator+(const ref<String> &other) const
{
    std::shared_ptr<String> ptr = std::make_shared<String>();
    *ptr = *(*this) + *other;
    return ref<String>(ptr);
}

ref<String> ref<String>::operator+(ref<Object> object) const
{
    std::shared_ptr<String> ptr = std::make_shared<String>();
    *ptr = *(*this) + *(object->toString());
    return ref<String>(ptr);
}

size_t String::find(ref<String> pattern) const { return std::string::find(*pattern); }

size_t String::find_first_of(ref<String> pattern) const { return std::string::find_first_of(*pattern); }

size_t String::find_first_not_of(ref<String> pattern) const { return std::string::find_first_not_of(*pattern); }

size_t String::find_last_of(ref<String> pattern) const { return std::string::find_last_of(*pattern); }

size_t String::find_last_not_of(ref<String> pattern) const { return std::string::find_last_not_of(*pattern); }

bool String::isEmpty() const
{
    return this->empty();
}

bool String::isNotEmpty() const
{
    return !this->isEmpty();
}

bool String::startsWith(ref<String> prefix) const
{
    if (prefix->size() > this->size())
        return false;
    return std::equal(prefix->begin(), prefix->end(), this->begin());
}

bool String::endsWith(ref<String> suffix) const
{
    if (suffix->size() > this->size())
        return false;
    return std::equal(suffix->rbegin(), suffix->rend(), this->rbegin());
}

ref<String> String::substr(size_t begin, size_t length) const
{
    return std::make_shared<String>(&this->operator[](begin), length);
}

std::ostream &operator<<(std::ostream &os, const ref<String> &str)
{
    return os << *str;
}

std::istream &operator>>(std::istream &is, ref<String> &str)
{
    std::shared_ptr<String> ptr = std::make_shared<String>();
    is >> *ptr;
    str = ptr;
    return is;
}

ref<String> getline(std::istream &is)
{
    std::shared_ptr<String> ptr = std::make_shared<String>();
    getline(is, *ptr);
    return ref<String>(ptr);
}

ref<String> operator+(const char c, const ref<String> &string)
{
    std::shared_ptr<String> ptr = std::make_shared<String>();
    *ptr = std::to_string(c) + *string;
    return ref<String>(ptr);
}

ref<String> operator+(const char *const str, const ref<String> &string)
{
    std::shared_ptr<String> ptr = std::make_shared<String>();
    *ptr = std::string(str) + *string;
    return ref<String>(ptr);
}

