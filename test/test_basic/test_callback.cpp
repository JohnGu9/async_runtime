#include <iostream>
#include "framework/basic/value_notifier.h"

void _test()
{
    std::cout << "This is _test function" << std::endl;
}

int main(const int argc, char **args)
{
    ValueNotifier<bool> notifier(false);
    auto fn = VoidCallBack::fromFunction(_test);
    notifier.addListener(fn);

    std::cout << "Change value" << std::endl;
    notifier.setValue(true);

    std::cout << "Not change value" << std::endl;
    notifier.setValue(true);

    std::cout << "Dispose" << std::endl;
    notifier.dispose();

    std::cout << "Invalid access after dispose()" << std::endl;
    notifier.setValue(false);

    return EXIT_SUCCESS;
}