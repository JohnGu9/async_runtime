#pragma once
#include <iostream>

// not work in Windows CMD
#define RESET "\033[0m"
#define BLACK "\033[30m"              /* Black */
#define RED "\033[31m"                /* Red */
#define GREEN "\033[32m"              /* Green */
#define YELLOW "\033[33m"             /* Yellow */
#define BLUE "\033[34m"               /* Blue */
#define MAGENTA "\033[35m"            /* Magenta */
#define CYAN "\033[36m"               /* Cyan */
#define WHITE "\033[37m"              /* White */
#define BOLDBLACK "\033[1m\033[30m"   /* Bold Black */
#define BOLDRED "\033[1m\033[31m"     /* Bold Red */
#define BOLDGREEN "\033[1m\033[32m"   /* Bold Green */
#define BOLDYELLOW "\033[1m\033[33m"  /* Bold Yellow */
#define BOLDBLUE "\033[1m\033[34m"    /* Bold Blue */
#define BOLDMAGENTA "\033[1m\033[35m" /* Bold Magenta */
#define BOLDCYAN "\033[1m\033[36m"    /* Bold Cyan */
#define BOLDWHITE "\033[1m\033[37m"   /* Bold White */

#ifndef debug_print
#ifndef NDEBUG
#define debug_print(x) std::cout << "[" << BOLDBLUE << "DEBUG" << RESET << "] " << x << std::endl
#else
#define debug_print(x) void(0)
#endif
#endif

#ifndef info_print
#define info_print(x) std::cout << "[" << BOLDGREEN << "INFO " << RESET << "] " << x << std::endl
#endif

#ifndef warning_print
#define warning_print(x) std::cout << "[" << BOLDYELLOW << "WARNING" << RESET << "] " << x << std::endl
#endif

#ifndef error_print
#define error_print(x) std::cout << "[" << BOLDRED << "ERROR" << RESET << "] " << x << std::endl
#endif

#ifndef font_wrapper
#define font_wrapper(font, x) font << x << RESET
#endif
