#include "async_runtime/object/object.h"

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
