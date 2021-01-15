#include "async_runtime.h"

void onCommand(const std::string &in, Object::Ref<RootElement> &root);

void runApp(Object::Ref<Widget> widget)
{
    debug_print("Debug mode on");
    debug_print(font_wrapper(BOLDCYAN, "AsyncRuntime") << " connecting");
    debug_print("Root widget is \"" << widget->toString() << "\"");

#ifndef DEBUG
    printf("\033c");
    info_print("Debug mode off");
#endif

    Object::Ref<RootElement> root = Object::create<RootElement>(widget);
    root->attach();
    {
        std::stringstream ss;
        ss << font_wrapper(BOLDCYAN, "AsyncRuntime") << " is ready";
        root->getStdoutHandler()->writeLine(ss.str());
    }
    {
        std::stringstream ss;
        ss << "Enter '"
           << font_wrapper(BOLDBLUE, 'q')
           << "' to quit, '"
           << font_wrapper(BOLDBLUE, "-h")
           << "' or '"
           << font_wrapper(BOLDBLUE, "--help")
           << "' for more information";
        root->getStdoutHandler()->writeLine(ss.str());
    }

    std::string input;
    while (root->getStdoutHandler()->write(">> ").get() && std::getline(std::cin, input))
    {
        if (input == "q" || input == "quit")
        {
            std::stringstream ss;
            ss << "Sure to quit (" << font_wrapper(BOLDBLUE, 'y') << '/' << font_wrapper(BOLDRED, "n") << " default is n)? ";
            root->getStdoutHandler()->write(ss.str()).get();
            if (std::getline(std::cin, input) && (input == "y" || input == "yes"))
                break;
            root->getStdoutHandler()->writeLine("cancel").get();
        }
        else
            onCommand(input, root);
    }

    root->detach();
    info_print(font_wrapper(BOLDCYAN, "AsyncRuntime") << " shutdown");
}

void onCommand(const std::string &in, Object::Ref<RootElement> &root)
{
    Logger::Handler handler = root->getStdoutHandler();
    if (in == "-h" || in == "--help")
    {
#define help_format(x, y) " " << x << "\t\t" << y
        std::stringstream ss;
        ss << std::endl;
        ss << help_format(BOLDWHITE << "command", "arguments" << RESET) << std::endl;
        ss << help_format("-------", "---------") << std::endl;
        ss << help_format("ls\t", "()") << std::endl;
        ss << help_format("ps\t", "()") << std::endl;
        ss << help_format("pwd\t", "()") << std::endl;
        ss << help_format("reassembly", "()") << std::endl;
        handler->write(ss.str());
        return;
    }
    else if (in.empty())
        return;

    auto commandLength = in.find(" ");
    auto command = in.substr(0, commandLength == std::string::npos ? in.size() : commandLength);
    if (command == "clear")
    {
        printf("\033c");
    }
    else if (command == "ls")
    {
        Object::Map<Object::RuntimeType, Object::List<size_t>> map;
        root->visitDescendant([&map](Object::Ref<Element> element) -> bool {
            Object::RuntimeType type = element->runtimeType();
            if (map.find(type) == map.end())
                map[type] = {};
            map[type].push_back((size_t)(element.get()));
            return false;
        });
        std::stringstream ss;
        for (auto iter = map.begin(); iter != map.end(); iter++)
        {
            ss << font_wrapper(BOLDGREEN, iter->first) << "[ ";
            for (auto i : iter->second)
                ss << i << " ";
            ss << "]  ";
        }
        handler->writeLine(ss.str());
    }
    else if (command == "ps")
    {
    }
    else if (command == "pwd")
    {
    }
    else if (command == "reassembly")
    {
        root->detach();
        root->attach();
    }
    else
    {
        error_print("No such method");
        info_print("'-h' or '--help' for more information");
    }
}
