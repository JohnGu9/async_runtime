#include "async_runtime/utilities/number.h"

static auto integer_hash = std::hash<long long>();
static auto double_hash = std::hash<double>();

// Short
Number::Type::Value Number::Short::type() { return Number::Type::Short; }
size_t Number::Short::hashCode() { return integer_hash(value); }

// Integer
Number::Type::Value Number::Integer::type() { return Number::Type::Integer; }
size_t Number::Integer::hashCode() { return integer_hash(value); }

// Long
Number::Type::Value Number::Long::type() { return Number::Type::Long; }
size_t Number::Long::hashCode() { return integer_hash(value); }

// LongLong
Number::Type::Value Number::LongLong::type() { return Number::Type::LongLong; }
size_t Number::LongLong::hashCode() { return integer_hash(value); }

// Float
Number::Type::Value Number::Float::type() { return Number::Type::Float; }
size_t Number::Float::hashCode() { return double_hash(value); }

// Double
Number::Type::Value Number::Double::type() { return Number::Type::Double; }
size_t Number::Double::hashCode() { return double_hash(value); }
