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

    bool isNative() override { return false; }

    const char *const c_str() const override { return _parent->c_str() + _begin; }
    const char *const data() const override { return _parent->data() + _begin; }
    size_t length() const override { return _length; }
    size_t size() const override { return _length; }

    const char &operator[](size_t index) const override
    {
        if (index >= _length)
            throw std::range_error("String::View::operator[] access out of range memory");
        return _parent[index + _begin];
    }
    const_iterator begin() const override { return _parent->begin() + _begin; }
    const_iterator end() const override { return _parent->begin() + _end; }
    const_reverse_iterator rbegin() const override { return _parent->rbegin() + (_parent->length() - _end); }
    const_reverse_iterator rend() const override { return _parent->rbegin() + (_parent->length() - _begin); }

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
    DEBUG_ASSERT(_parent->isNative());
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
    return std::equal((*this)->begin(), (*this)->end(), other.begin());
}

bool ref<String>::operator==(std::string &&other) const
{
    if ((*this)->length() != other.length())
        return false;
    return std::equal((*this)->begin(), (*this)->end(), other.begin());
}

bool String::operator==(ref<Object> other)
{
    auto ptr = other.get();
    if (String *string = dynamic_cast<String *>(ptr))
    {
        if (this->length() != string->length())
            return false;
        return std::equal(this->begin(), this->end(), string->begin());
    }
    return false;
}

bool String::operator==(const ref<String> &string)
{
    if (this->length() != string->length())
        return false;
    return std::equal(this->begin(), this->end(), string->begin());
}

void String::toStringStream(std::ostream &os)
{
    os.write(this->data(), this->length());
}

ref<String> String::toString()
{
    return self();
}

bool String::any(Function<bool(const char &)> fn) const
{
    for (const auto &iter : *this)
        if (fn(iter))
            return true;
    return false;
}

bool String::every(Function<bool(const char &)> fn) const
{
    for (const auto &iter : *this)
        if (!fn(iter))
            return false;
    return true;
}

void String::forEach(Function<void(const char &)> fn) const
{
    for (const auto &iter : *this)
        fn(iter);
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
    if (prefix->length() > this->length())
        return false;
    return std::equal(prefix->begin(), prefix->end(), this->begin());
}

bool String::endsWith(ref<String> suffix) const
{
    if (suffix->length() > this->length())
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
    std::transform(begin(), end(), result.begin(), [](unsigned char c)
                   { return std::tolower(c); });
    return std::move(result);
}

ref<String> String::toUpperCase() const
{
    std::string result;
    result.resize(this->length());
    std::transform(begin(), end(), result.begin(), [](unsigned char c)
                   { return std::toupper(c); });
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
