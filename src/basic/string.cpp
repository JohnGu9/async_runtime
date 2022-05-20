#include "async_runtime/basic/string.h"

template <>
bool operator==(const option<String> &lhs, const option<String> &rhs)
{
    lateref<String> lh;
    if (lhs.isNotNull(lh))
    {
        lateref<String> rh;
        if (rhs.isNotNull(rh))
            return lh == rh;
        return false;
    }
    else
        return rhs == nullptr;
}

template <>
bool operator!=(const option<String> &lhs, const option<String> &rhs)
{
    return !(lhs == rhs);
}

bool ref<String>::operator==(const ref<String> &other) const
{
    if (this->get() == other.get())
        return true;
    if ((*this)->length() != other->length())
        return false;
    return std::equal((*this)->begin(), (*this)->end(), other->begin());
}

ref<String> ref<String>::operator+(const char c) const
{
    auto res = *(*this) + c;
    return res;
}

ref<String> ref<String>::operator+(const char *const str) const
{
    auto res = *(*this) + str;
    return res;
}

ref<String> ref<String>::operator+(const std::string &other) const
{
    auto res = *(*this) + other;
    return res;
}

ref<String> ref<String>::operator+(std::string &&other) const
{
    auto res = *(*this) + other;
    return res;
}

ref<String> ref<String>::operator+(const ref<String> &other) const
{
    auto res = *(*this) + *other;
    return res;
}

ref<String> ref<String>::operator+(ref<Object> object) const
{
    auto res = *(*this) + *(object->toString());
    return res;
}

size_t String::find(ref<String> pattern, size_t start) const { return std::string::find(*pattern, start); }

size_t String::find_first_of(ref<String> pattern, size_t start) const { return std::string::find_first_of(*pattern, start); }

size_t String::find_first_not_of(ref<String> pattern, size_t start) const { return std::string::find_first_not_of(*pattern, start); }

size_t String::find_last_of(ref<String> pattern, size_t start) const { return std::string::find_last_of(*pattern, start); }

size_t String::find_last_not_of(ref<String> pattern, size_t start) const { return std::string::find_last_not_of(*pattern, start); }

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

ref<List<ref<String>>> String::split(ref<String> pattern) const
{
    ref<List<ref<String>>> list = Object::create<List<ref<String>>>();
    size_t lastIndex = 0;
    while (true)
    {
        auto index = this->find(pattern, lastIndex);
        if (index == String::npos)
        {
            auto start = std::string::begin() + lastIndex;
            auto end = std::string::end();
            if (start != end)
                list->emplace_back(std::string(start, end));
            break;
        }
        else if (index != lastIndex)
        {
            list->emplace_back(std::string(std::string::begin() + lastIndex, std::string::begin() + index));
        }
        lastIndex = index + pattern->length();
    }
    return list;
}

ref<String> String::substr(size_t begin, size_t length) const
{
    return std::make_shared<String>(std::string::substr(begin, length));
}

std::ostream &operator<<(std::ostream &os, const ref<String> &str)
{
    return os << *str;
}

std::ostream &operator<<(std::ostream &os, ref<String> &&str)
{
    return os << *str;
}

ref<String> getline(std::istream &is)
{
    std::shared_ptr<String> ptr = std::make_shared<String>();
    std::getline(is, *ptr);
    return ref<String>(ptr);
}

ref<String> operator+(const char c, const ref<String> &string)
{
    auto res = std::to_string(c) + *string;
    return res;
}

ref<String> operator+(const char *const str, const ref<String> &string)
{
    auto res = std::string(str) + *string;
    return res;
}
