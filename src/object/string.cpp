#include "async_runtime/object/object.h"
#include <algorithm>
#include <cctype>

class String::View : public String
{
    const ref<String> _parent;
    const size_t _begin;
    const size_t _end;
    const size_t _length;

    using super = String;

public:
    View(ref<String> parent, size_t begin, size_t end);

    const char *const c_str() const override { return _parent->c_str() + _begin; }
    const char *const data() const override { return _parent->data() + _begin; }
    size_t length() const override { return _length; }
    size_t size() const override { return _length; }

    const char &operator[](const size_t &index) const override
    {
        if (index >= _length)
            throw std::range_error("String::View::operator[] access out of range memory");
        return _parent[index + _begin];
    }
    ref<ConstIterator<char>> begin() const override { return _parent->begin().get()->covariant<String::StringConstIterator>()->operator+(_begin); }
    ref<ConstIterator<char>> end() const override { return _parent->begin().get()->covariant<String::StringConstIterator>()->operator+(_end); }
    ref<ConstIterator<char>> rbegin() const override { return _parent->rbegin().get()->covariant<String::ReverseConstIterator>()->operator+(_parent->length() - _end); }
    ref<ConstIterator<char>> rend() const override { return _parent->rbegin().get()->covariant<String::ReverseConstIterator>()->operator+(_parent->length() - _begin); }

    size_t find(ref<String> pattern, size_t start = 0) const override
    {
        size_t result = _parent->find(pattern, start + _begin);
        if (result >= _end)
            return super::npos;
        return result - _begin;
    }
    size_t findFirstOf(ref<String> pattern, size_t start = 0) const override
    {
        size_t result = _parent->findFirstOf(pattern, start + _begin);
        if (result >= _end)
            return super::npos;
        return result - _begin;
    }
    size_t findFirstNotOf(ref<String> pattern, size_t start = 0) const override
    {
        size_t result = _parent->findFirstNotOf(pattern, start + _begin);
        if (result >= _end)
            return super::npos;
        return result - _begin;
    }
    size_t findLastOf(ref<String> pattern, size_t start = npos) const override
    {
        const auto max = std::max(_begin, _end - 1);
        start = start == npos ? max : std::min(start + _begin, max);
        size_t result = _parent->findLastOf(pattern, start);
        if (result < _begin)
            return super::npos;
        return result - _begin;
    }
    size_t findLastNotOf(ref<String> pattern, size_t start = npos) const override
    {
        const auto max = std::max(_begin, _end - 1);
        start = start == npos ? max : std::min(start + _begin, max);
        size_t result = _parent->findLastNotOf(pattern, start);
        if (result < _begin)
            return super::npos;
        return result - _begin;
    }

    ref<String> substr(size_t begin = 0, size_t length = SIZE_MAX) const override;
};

String::View::View(ref<String> parent, size_t begin, size_t end)
    : _parent(parent), _begin(begin), _end(end), _length(_end - _begin)
{
    DEBUG_ASSERT(_parent->cast<String::View>() == nullptr);
    DEBUG_ASSERT(_parent->length() >= _begin && _parent->length() >= _end);
}

ref<String> String::View::substr(size_t begin, size_t length) const
{
    begin = begin + this->_begin;
    return Object::create<String::View>(this->_parent, begin, length == npos ? this->length() + begin : length + begin);
}

bool ref<String>::operator==(const char *const other) const
{
    const auto len = std::strlen(other);
    if (len != (*this)->length())
        return false;
    return std::strncmp((*this)->data(), other, len) == 0;
}

bool ref<String>::operator==(const std::string &other) const
{
    if ((*this)->length() != other.length())
        return false;
    return std::strncmp((*this)->data(), other.data(), other.length()) == 0;
}

bool ref<String>::operator==(std::string &&other) const
{
    return this->operator==(static_cast<const std::string &>(other));
}

bool String::operator==(ref<Object> other)
{
    auto ptr = other.get();
    if (String *string = dynamic_cast<String *>(ptr))
    {
        if (this->length() != string->length())
            return false;
        return std::strncmp(this->data(), string->data(), string->length()) == 0;
    }
    return false;
}

bool String::operator==(const ref<String> &string)
{
    if (this->length() != string->length())
        return false;
    return std::strncmp(this->data(), string->data(), string->length()) == 0;
}

void String::toStringStream(std::ostream &os)
{
    if (_async_runtime::OstreamStack::depth == 1)
        os.write(this->data(), this->length());
    else
    {
        os << '"';
        os.write(this->data(), this->length());
        os << '"';
    }
}

ref<String> String::toString()
{
    return self();
}

static size_t hash_c_string(const char *p, size_t s)
{
    static const size_t prime = 31;
    size_t result = 0;
    for (size_t i = 0; i < s; ++i)
        result = p[i] + (result * prime);
    return result;
}

size_t String::hashCode()
{
    return hash_c_string(this->data(), this->length());
}

bool String::any(Function<bool(const char &)> fn) const
{
    for (size_t i = 0, len = this->length(); i < len; i++)
        if (fn(this->operator[](i)))
            return true;
    return false;
}

bool String::every(Function<bool(const char &)> fn) const
{
    for (size_t i = 0, len = this->length(); i < len; i++)
        if (!fn(this->operator[](i)))
            return false;
    return true;
}

void String::forEach(Function<void(const char &)> fn) const
{
    for (size_t i = 0, len = this->length(); i < len; i++)
        fn(this->operator[](i));
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
    return super::find(pattern->data(), start, pattern->length());
}

size_t String::findFirstOf(ref<String> pattern, size_t start) const
{
    return super::find_first_of(pattern->data(), start, pattern->length());
}

size_t String::findFirstNotOf(ref<String> pattern, size_t start) const
{
    return super::find_first_not_of(pattern->data(), start, pattern->length());
}

size_t String::findLastOf(ref<String> pattern, size_t start) const
{
    return super::find_last_of(pattern->data(), start, pattern->length());
}

size_t String::findLastNotOf(ref<String> pattern, size_t start) const
{
    return super::find_last_not_of(pattern->data(), start, pattern->length());
}

std::string String::toStdString() const
{
    return std::string(this->data(), this->length());
}

bool String::startsWith(ref<String> prefix) const
{
    if (prefix->length() > this->length())
        return false;
    return std::strncmp(this->data(), prefix->data(), prefix->length()) == 0;
}

bool String::endsWith(ref<String> suffix) const
{
    if (suffix->length() > this->length())
        return false;
    return std::strncmp((this->data() + this->length() - suffix->length()), suffix->data(), suffix->length()) == 0;
}

ref<List<ref<String>>> String::split(ref<String> pattern) const
{
    auto list = List<ref<String>>::create();
    size_t lastIndex = 0;
    while (true)
    {
        auto index = this->find(pattern, lastIndex);
        if (index == String::npos)
        {
            auto start = lastIndex;
            auto length = this->length() - lastIndex;
            if (length > 0)
                list->emplace(this->substr(lastIndex, length));
            break;
        }
        else if (index != lastIndex)
        {
            list->emplace(this->substr(lastIndex, index - lastIndex));
        }
        lastIndex = index + pattern->length();
    }
    return list;
}

ref<String> String::trim() const
{
    auto begin = this->findFirstNotOf(' ');
    auto end = this->findLastNotOf(' ') + 1;
    return this->substr(begin, end - begin);
}

ref<String> String::trim(ref<String> pattern) const
{
    auto begin = this->findFirstNotOf(pattern);
    auto end = this->findLastNotOf(pattern) + 1;
    return this->substr(begin, end - begin);
}

ref<String> String::toLowerCase() const
{
    std::string result;
    result.resize(this->length());
    for (size_t i = 0, len = this->length(); i < len; i++)
        result[i] = std::tolower(this->operator[](i));
    return std::move(result);
}

ref<String> String::toUpperCase() const
{
    std::string result;
    result.resize(this->length());
    for (size_t i = 0, len = this->length(); i < len; i++)
        result[i] = std::toupper(this->operator[](i));
    return std::move(result);
}

ref<String> String::substr(size_t begin, size_t length) const
{
    auto self = Object::cast<>(const_cast<String *>(this));
    return Object::create<String::View>(self, begin, length == npos ? this->length() : (length + begin));
}

ref<String> String::replace(size_t begin, size_t length, ref<String> other) const
{
    return String::connect(this->substr(0, begin), other, this->substr(begin + length));
}

ref<String> String::reverse() const
{
    std::string buffer;
    buffer.resize(this->length());
    for (size_t i = 0, r = this->length() - 1;
         i != this->length(); i++, r--)
        buffer[i] = this->operator[](r);
    return std::move(buffer);
}

ref<String> String::getline(std::istream &is)
{
    std::string str;
    std::getline(is, str);
    return str;
}

namespace _async_runtime
{
    namespace OstreamStack
    {
        std::recursive_mutex mutex;
        size_t depth(0);
    };
};
