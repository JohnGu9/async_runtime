#pragma once

#include "../object/object.h"

// declare state first for easy reference order later
// these classes will be implement in "stateful_widget.h"

class StatefulWidget;

template <typename T = StatefulWidget, typename Object::Base<StatefulWidget>::isBaseOf<T>::type * = nullptr>
class State;
