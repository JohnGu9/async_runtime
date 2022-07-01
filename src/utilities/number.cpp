#include "async_runtime/utilities/number.h"

// Short
Number::Type::Value Number::Short::type() { return Number::Type::Short; }
size_t Number::Short::hashCode() { return std::hash<long long>()(value); }

// Integer
Number::Type::Value Number::Integer::type() { return Number::Type::Integer; }
size_t Number::Integer::hashCode() { return std::hash<long long>()(value); }

// Long
Number::Type::Value Number::Long::type() { return Number::Type::Long; }
size_t Number::Long::hashCode() { return std::hash<long long>()(value); }

// LongLong
Number::Type::Value Number::LongLong::type() { return Number::Type::LongLong; }
size_t Number::LongLong::hashCode() { return std::hash<long long>()(value); }

// Float
Number::Type::Value Number::Float::type() { return Number::Type::Float; }
size_t Number::Float::hashCode() { return std::hash<double>()(value); }

// Double
Number::Type::Value Number::Double::type() { return Number::Type::Double; }
size_t Number::Double::hashCode() { return std::hash<double>()(value); }
