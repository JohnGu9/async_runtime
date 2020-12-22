#include <string>
#include <iostream>
#include <sstream>
#include "test.h"

int main(int argc, char **args)
{
    auto widget = Object::create<MainActivity>();
    runApp(widget);
    return EXIT_SUCCESS;
}
