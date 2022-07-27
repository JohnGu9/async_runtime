#pragma once
#include <type_traits>

// declare state first for easy reference order later
// these classes will be implement in "stateful_widget.h"

class StatefulWidget;

template <typename T = StatefulWidget, typename std::enable_if<std::is_base_of<StatefulWidget, T>::value>::type * = nullptr>
class State;
