#include "async_runtime/utilities/number.h"
#include <cmath>
#include <limits>

// SizeType
Number::Type::Value Number::SizeType::type() { return Number::Type::SizeType; }
size_t Number::SizeType::hashCode() { return this->value; }

bool Number::SizeType::operator==(const size_t &other) { return this->value == other; }
bool Number::SizeType::operator==(const short &other) { return Number::cmp<>(this->value, other) == 0; }
bool Number::SizeType::operator==(const int &other) { return Number::cmp<>(this->value, other) == 0; }
bool Number::SizeType::operator==(const long &other) { return Number::cmp<>(this->value, other) == 0; }
bool Number::SizeType::operator==(const long long &other) { return Number::cmp<>(this->value, other) == 0; }

bool Number::SizeType::operator<(const size_t &other) { return this->value < other; }
bool Number::SizeType::operator<(const short &other) { return Number::cmp<>(this->value, other) < 0; }
bool Number::SizeType::operator<(const int &other) { return Number::cmp<>(this->value, other) < 0; }
bool Number::SizeType::operator<(const long &other) { return Number::cmp<>(this->value, other) < 0; }
bool Number::SizeType::operator<(const long long &other) { return Number::cmp<>(this->value, other) < 0; }

bool Number::SizeType::operator>(const size_t &other) { return this->value < other; }
bool Number::SizeType::operator>(const short &other) { return Number::cmp<>(this->value, other) > 0; }
bool Number::SizeType::operator>(const int &other) { return Number::cmp<>(this->value, other) > 0; }
bool Number::SizeType::operator>(const long &other) { return Number::cmp<>(this->value, other) > 0; }
bool Number::SizeType::operator>(const long long &other) { return Number::cmp<>(this->value, other) > 0; }

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
