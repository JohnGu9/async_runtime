#include <iostream>
#include "async_runtime.h"

void runApp(Object::Ref<Widget> widget)
{
    std::cout << "-- Async runtime connecting... " << std::endl;
    std::cout << "-- Scheduler started " << std::endl;
    std::cout << "-- Root widget is \"" << widget->toString() << "\"" << std::endl;
    std::cout << "-- Async runtime connected " << std::endl;
    Object::Ref<RootElement> root = Object::create<RootElement>(widget);
    root->build();

    {
        std::cout << "** Input \"Q\" to quit " << std::endl;
        std::string input;
        while (getline(std::cin, input))
        {
            std::cout << input << std::endl;
            if (input == "q")
            {
                std::cout << "Request to quit" << std::endl;
                break;
            }
        }
    }

    std::cout << "-- Async runtime shutdown " << std::endl;
}