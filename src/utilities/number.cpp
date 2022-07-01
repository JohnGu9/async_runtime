#include "async_runtime/utilities/number.h"

// Short
Number::Type::Value Number::Short::type() { return Number::Type::Short; }

// Integer
Number::Type::Value Number::Integer::type() { return Number::Type::Integer; }

// Long
Number::Type::Value Number::Long::type() { return Number::Type::Long; }

// LongLong
Number::Type::Value Number::LongLong::type() { return Number::Type::LongLong; }

// Float
Number::Type::Value Number::Float::type() { return Number::Type::Float; }

// Double
Number::Type::Value Number::Double::type() { return Number::Type::Double; }
