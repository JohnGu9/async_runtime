#pragma once

// export library
#include "framework/widgets/widget.h"
#include "framework/fundamental/scheduler.h"

void runApp(Object::Ref<Widget> widget);
void onCommand(const std::string &in, Object::Ref<RootElement> &root);