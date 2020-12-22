#pragma once

// export library
#include "framework/widgets/widget.h"

#define print(x) std::cout << x << std::endl;

#define debug_print(x) std::cout << "[" << BLUE << "DEBUG" << RESET << "] " << x << std::endl;
#define info_print(x) std::cout << "[" << GREEN << "INFO " << RESET << "] " << x << std::endl;
#define warning_print(x) std::cout << "[" << YELLOW << "WARNING" << RESET << "] " << x << std::endl;
#define error_print(x) std::cout << "[" << RED << "ERROR" << RESET << "] " << x << std::endl;

void runApp(Object::Ref<Widget> widget);

void onCommand(const std::string &in, Object::Ref<RootElement> &root);