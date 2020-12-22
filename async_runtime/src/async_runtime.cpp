#include <iostream>
#include "async_runtime.h"

void runApp(Object::Ref<Widget> widget)
{
#ifdef DEBUG
    debug_print("Debug mode on");
    debug_print("Async runtime connecting");
    debug_print("Scheduler started");
    debug_print("Root widget is \"" << widget->toString() << "\"");
#elif
    info_print("Debug mode off");
#endif
    info_print("Async runtime is ready");

    Object::Ref<RootElement> root = Object::create<RootElement>(widget);
    root->attach();
    root->build();

    std::cout << BLUE << "-- input "
              << RESET
              << "\"quit\""
              << BLUE
              << " to exit, '-h' or '--help' for more information. "
              << RESET << std::endl;

    std::string input;
    while (std::cout << ">> " && getline(std::cin, input))
    {
        if (input == "quit" || input == "q")
        {
            warning_print("Request to quit");
            break;
        }
        onCommand(input, root);
    }

    root->detach();

    info_print("Async runtime shutdown");
}

void onCommand(const std::string &in, Object::Ref<RootElement> &root)
{
#define help_format(x, y) " " << x << "\t\t" << y

    if (in == "-h" || in == "--help")
    {
        std::cout << help_format("command", "arguments") << std::endl
                  << std::endl;
        std::cout << help_format("print_tree", "()") << std::endl;
        std::cout << help_format("reassembly", "()") << std::endl;
        return;
    }

    std::string::size_type commandLength = in.find(" ");
    std::string command = in.substr(0, commandLength == std::string::npos ? in.size() : commandLength);
    if (command == "print_tree")
    {
    }
    else if (command == "reassembly")
    {
        root->detach();
        root->attach();
        root->build();
    }
    else
    {
        error_print("No such method. ");
        info_print("'-h' or '--help' for more information. ");
    }
}