#include "async_runtime/object/object.h"

bool ref<String>::operator==(const char *const other) const
{
    const auto len = strlen(other);
    if (len != (*this)->length())
        return false;
    return strncmp((*this)->data(), other, len) == 0;
}

bool ref<String>::operator==(const std::string &other) const
{
    if ((*this)->length() != other.length())
        return false;
    return std::equal((*this)->begin(), (*this)->end(), other.begin());
}

bool ref<String>::operator==(std::string &&other) const
{
    if ((*this)->length() != other.length())
        return false;
    return std::equal((*this)->begin(), (*this)->end(), other.begin());
}

ref<String> operator+(const char c, ref<String> string)
{
    return String::connect(c, string);
}

ref<String> operator+(const char *const str, ref<String> string)
{
    return String::connect(str, string);
}

ref<String> operator+(const std::string &str, ref<String> string)
{
    return String::connect(str, string);
}

ref<String> operator+(std::string &&str, ref<String> string)
{
    return String::connect(str, string);
}

size_t String::find(ref<String> pattern, size_t start) const
{
    return std::string::find(pattern->data(), start, pattern->length());
}

size_t String::find_first_of(ref<String> pattern, size_t start) const
{
    return std::string::find_first_of(pattern->data(), start, pattern->length());
}

size_t String::find_first_not_of(ref<String> pattern, size_t start) const
{
    return std::string::find_first_not_of(pattern->data(), start, pattern->length());
}

size_t String::find_last_of(ref<String> pattern, size_t start) const
{
    return std::string::find_last_of(pattern->data(), start, pattern->length());
}

size_t String::find_last_not_of(ref<String> pattern, size_t start) const
{
    return std::string::find_last_not_of(pattern->data(), start, pattern->length());
}

std::string String::toStdString() const
{
    return std::string(this->data(), this->length());
}

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
            auto start = lastIndex;
            auto length = this->length() - lastIndex;
            if (length > 0)
                list->emplace_back(this->substr(lastIndex, length));
            break;
        }
        else if (index != lastIndex)
        {
            list->emplace_back(this->substr(lastIndex, index - lastIndex));
        }
        lastIndex = index + pattern->length();
    }
    return list;
}

ref<String> String::trim() const
{
    auto begin = this->find_first_not_of(' ');
    auto end = this->find_last_not_of(' ') + 1;
    return this->substr(begin, end - begin);
}

ref<String> String::substr(size_t begin, size_t length) const
{
    auto self = Object::cast<>(const_cast<String *>(this));
    return Object::create<String::View>(self, begin, std::min(length + begin, this->length()));
}

std::ostream &operator<<(std::ostream &os, const ref<String> &str)
{
    return os.write(str->data(), str->length());
}

std::ostream &operator<<(std::ostream &os, ref<String> &&str)
{
    return os.write(str->data(), str->length());
}

ref<String> String::getline(std::istream &is)
{
    std::string str;
    std::getline(is, str);
    return str;
}

String::View::View(ref<String> parent, size_t begin, size_t end)
    : _parent(parent), _begin(begin), _end(end), _length(_end - _begin)
{
    if (!parent->isNative()) // [[unlikely]]
    {
        auto view = parent->covariant<String::View>();
        const_cast<ref<String> &>(this->_parent) = view->_parent;
        const_cast<size_t &>(this->_begin) = view->_begin + begin;
        const_cast<size_t &>(this->_end) = this->_begin + _length;
    }

#ifndef NDEBUG
    auto parentLength = _parent->length();
    assert(parentLength >= _begin && parentLength >= _end);
#endif
}