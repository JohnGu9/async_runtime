#include <iostream>
#include "framework/basic/value_notifier.h"

void _test(Object::Ref<Listenable> self)
{
    Object::Ref<ValueNotifier<bool>> notifier = self->cast<ValueNotifier<bool>>();
    std::cout << "Value changed to " << notifier->getValue() << std::endl;
}

int main(const int argc, char **args)
{
    auto notifier = Object::create<ValueNotifier<bool>>(false);
    auto fn = Object::create<Fn<void(Object::Ref<Listenable>)>>(_test);
    notifier->addListener(fn);

    std::cout << "Change value" << std::endl;
    notifier->setValue(true);

    std::cout << "Not change value" << std::endl;
    notifier->setValue(true);

    std::cout << "Remove Listener and change value" << std::endl;
    notifier->removeListener(fn);
    notifier->setValue(false);

    std::cout << "Dispose" << std::endl;
    notifier->dispose();

    std::cout << "Invalid access after dispose()" << std::endl;
    notifier->setValue(false);

    return EXIT_SUCCESS;
}