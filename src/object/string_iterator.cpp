#include "async_runtime/object/object.h"

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
