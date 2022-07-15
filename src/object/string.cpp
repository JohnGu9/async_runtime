#include "async_runtime/object/object.h"
#include <algorithm>
#include <cctype>

class String::View : public String::Static
{
    using super = String::Static;

public:
    const ref<String> parent;
    View(ref<String> parent, const char *const pointer, const size_t len);
    ref<String> substr(size_t begin = 0, size_t length = SIZE_MAX) const override;
};

String::View::View(ref<String> parent, const char *const pointer, const size_t len)
    : super(pointer, len), parent(parent)
{
    DEBUG_ASSERT(parent->cast<String::View>() == nullptr);
    DEBUG_ASSERT(parent->data() <= pointer && parent->length() >= len);
}

ref<String> String::View::substr(size_t begin, size_t length) const
{
    auto left = this->len - begin;
    if (length > left)
        length = left;
    return Object::create<String::View>(this->parent, this->data() + begin, length);
}

const ref<Map<size_t, ref<String>>> String::_staticCache = Map<size_t, ref<String>>::create();

ref<ConstIterator<char>> String::begin() const { return Object::create<StringConstIterator>(this->data(), 0); }
ref<ConstIterator<char>> String::end() const { return Object::create<StringConstIterator>(this->data(), this->length()); }
ref<ConstIterator<char>> String::rbegin() const { return Object::create<ReverseConstIterator>(this->data(), this->length() - 1); }
ref<ConstIterator<char>> String::rend() const { return Object::create<ReverseConstIterator>(this->data(), static_cast<size_t>(-1)); }

bool String::operator<(const ref<String> &other) const
{
    auto len = other->length();
    if (this->length() == len)
    {
        const char *a = this->data(), *b = other->data();
        for (size_t i = 0; i < len; i++)
        {
            if (a[i] == b[i])
                continue;
            return a[i] < b[i];
        }
        return false;
    }
    return this->length() < len;
}

bool String::operator>(const ref<String> &other) const
{
    auto len = other->length();
    if (this->length() == len)
    {
        const char *a = this->data(), *b = other->data();
        for (size_t i = 0; i < len; i++)
        {
            if (a[i] == b[i])
                continue;
            return a[i] > b[i];
        }
        return false;
    }
    return this->length() > len;
}

bool String::operator<(const option<String> &other) const
{
    if_not_null(other) return this->operator<(other);
    end_if() return false;
}

bool String::operator>(const option<String> &other) const
{
    if_not_null(other) return this->operator>(other);
    end_if() return true;
}

bool String::operator==(const option<Object> &other)
{
    if (auto ptr = dynamic_cast<String *>(other.get()))
    {
        if (this->length() != ptr->length())
            return false;
        return std::strncmp(this->data(), ptr->data(), ptr->length()) == 0;
    }
    return false;
}

bool String::operator==(const std::string &other) const
{
    auto len = other.length();
    if (this->length() != len)
        return false;
    return std::strncmp(this->data(), other.data(), len) == 0;
}

bool String::operator==(const char *const other) const
{
    auto start = this->data();
    size_t i = 0, len = this->length();
    for (; i < len; ++i)
    {
        if (start[i] != other[i])
            return false;
    }
    return other[i] == 0;
}

void String::toStringStream(std::ostream &os)
{
    if (Object::OstreamCallStack::depth == 1)
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

#include <mutex>

ref<String> String::_cacheLiterals(const char *c, size_t len)
{
    static std::mutex mutex;
    std::lock_guard<std::mutex> guard(mutex);
    auto it = String::_staticCache->findKey((size_t)c);
    if (it != String::_staticCache->end())
        return it.get()->value()->second;
    auto res = Object::create<String::Static>(c, len);
    String::_staticCache->emplace((size_t)c, res);
    return res;
}

ref<String> operator""_String(const char *c)
{
    return String::_cacheLiterals(c, strlen(c));
}

ref<String> operator""_String(const char *c, size_t len)
{
    return String::_cacheLiterals(c, len);
}

#include <vector>

static size_t samePrefixSuffix(const char *const data, size_t length)
{
    for (size_t i = 1; i < length; i++)
    {
        const auto compareLength = length - i;
        if (std::strncmp(data, data + i, compareLength) == 0)
            return compareLength;
    }
    return 0;
}

static std::vector<size_t> maxLengthSamePrefixSuffix(const char *const data, size_t length)
{
    std::vector<size_t> res(length, 0);
    for (size_t i = 1; i < length; i++)
        res[i] = samePrefixSuffix(data, i + 1);
    return res;
}

static size_t findFirstMatchChar(const char c, const char *const data, size_t start, size_t end)
{
    for (size_t i = start; i < end; i++)
    {
        if (data[i] == c)
            return i;
    }
    return String::npos;
}

size_t String::find(ref<String> pattern, size_t pos) const
{
    const auto parentLength = this->length(), patternLength = pattern->length();
    if ((parentLength - pos) >= patternLength)
    {
        const auto parentData = this->data(), patternData = pattern->data();
        const auto kmp = maxLengthSamePrefixSuffix(patternData, patternLength);
        for (size_t start = findFirstMatchChar(pattern[0], parentData, pos, parentLength - patternLength + 1), i = 1;
             start != String::npos;)
        {
            auto match = true;
            for (; i < patternLength; i++)
            {
                auto x = parentData + start + i,
                     y = patternData + i;
                if (*x != *y)
                {
                    match = false;
                    const auto hint = kmp[i - 1];
                    start += (i - hint);
                    i = hint;
                    break;
                }
            }
            if (match) // [[unlikely]]
                return start;
            else if (i == 0)
            {
                start = findFirstMatchChar(pattern[0], parentData, start, parentLength - patternLength + 1);
                i = 1;
            }
        }
    }
    return String::npos;
}

#include <set>

size_t String::findFirstOf(ref<String> pattern, size_t pos) const
{
    std::set<char> s(pattern->begin(), pattern->end());
    auto end = s.end();
    auto data = this->data();
    auto length = this->length();
    for (size_t i = pos; i < length; i++)
    {
        if (s.find(data[i]) != end)
            return i;
    }
    return String::npos;
}

size_t String::findFirstNotOf(ref<String> pattern, size_t pos) const
{
    std::set<char> s(pattern->begin(), pattern->end());
    auto end = s.end();
    auto data = this->data();
    auto length = this->length();
    for (size_t i = pos; i < length; i++)
    {
        if (s.find(data[i]) == end)
            return i;
    }
    return String::npos;
}

size_t String::findLastOf(ref<String> pattern, size_t pos) const
{
    auto length = this->length();
    if (pos == String::npos)
        pos = length - 1;
    auto data = this->data();
    std::set<char> s(pattern->begin(), pattern->end());
    auto end = s.end();
    for (size_t i = pos; i != static_cast<size_t>(-1); i--)
    {
        if (s.find(data[i]) != end)
            return i;
    }
    return String::npos;
}

size_t String::findLastNotOf(ref<String> pattern, size_t pos) const
{
    auto length = this->length();
    if (pos == String::npos)
        pos = length - 1;
    auto data = this->data();
    std::set<char> s(pattern->begin(), pattern->end());
    auto end = s.end();
    for (size_t i = pos; i != static_cast<size_t>(-1); i--)
    {
        if (s.find(data[i]) == end)
            return i;
    }
    return String::npos;
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
    auto begin = this->findFirstNotOf(" ");
    auto end = this->findLastNotOf(" ") + 1;
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
    auto left = this->length() - begin;
    if (length > left)
        length = left;
    return Object::create<String::View>(self, this->data() + begin, length);
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

bool String::IteratorBasic::operator==(const option<Object> &other)
{
    if (auto ptr = dynamic_cast<IteratorBasic *>(other.get()))
    {
        return this->data == ptr->data &&       // [[likely]]
               this->position == ptr->position; // [[unlikely]]
    }
    return false;
}

ref<ConstIterator<char>> String::StringConstIterator::next() const
{
    return Object::create<StringConstIterator>(data, position + 1);
}

ref<ConstIterator<char>> String::StringConstIterator::operator+(size_t shift) const
{
    return Object::create<StringConstIterator>(data, position + shift);
}

ref<ConstIterator<char>> String::ReverseConstIterator::next() const
{
    return Object::create<ReverseConstIterator>(data, position - 1);
}

ref<ConstIterator<char>> String::ReverseConstIterator::operator+(size_t shift) const
{
    return Object::create<ReverseConstIterator>(data, position - shift);
}
