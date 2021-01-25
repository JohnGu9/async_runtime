#pragma once

#include <type_traits>

class StatefulWidget;

template <typename T, typename std::enable_if<std::is_base_of<StatefulWidget, T>::value>::type * = nullptr>
class State;
