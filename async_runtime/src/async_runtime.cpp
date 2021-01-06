#include "async_runtime.h"

static std::mutex queue_mutex;
static std::condition_variable condition;

void runApp(Object::Ref<Widget> widget)
{
#ifdef DEBUG
    printf("\033c");
    debug_print("Debug mode on");
    debug_print(font_wrapper(BOLDCYAN, "AsyncRuntime") << " connecting");
    debug_print("Root widget is \"" << widget->toString() << "\"");
#elif
    info_print("Debug mode off");
#endif
    Object::Ref<RootElement> root = Object::create<RootElement>(widget);

    root->attach();

    info_print(font_wrapper(BOLDCYAN, "AsyncRuntime") << " is ready");

    info_print("Enter '"
               << font_wrapper(BOLDBLUE, "q")
               << "' to quit, '"
               << font_wrapper(BOLDBLUE, "-h")
               << "' or '"
               << font_wrapper(BOLDBLUE, "--help")
               << "' for more information");
    std::string input;
    onCommand("ls", root);

    while (std::cout << ">> " && std::getline(std::cin, input))
    {
        if (input == "q" || input == "quit")
        {
            warning_print("Sure to quit? (y)");
            if (getline(std::cin, input) && (input == "y" || input == "yes"))
                break;
            info_print("cancel");
        }
        onCommand(input, root);
    }

    root->detach();
    info_print(font_wrapper(BOLDCYAN, "AsyncRuntime") << " shutdown");
}

void onCommand(const std::string &in, Object::Ref<RootElement> &root)
{
#define help_format(x, y) " " << x << "\t\t" << y

    if (in == "-h" || in == "--help")
    {
        std::cout << help_format(BOLDWHITE << "command", "arguments" << RESET) << std::endl;
        std::cout << help_format("-------", "---------") << std::endl;
        std::cout << help_format("ls\t", "()") << std::endl;
        std::cout << help_format("ps\t", "()") << std::endl;
        std::cout << help_format("pwd\t", "()") << std::endl;
        std::cout << help_format("reassembly", "()") << std::endl;
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
        root->visitDescendant([&map](Object::Ref<Element> element) {
            Object::RuntimeType type = element->runtimeType();
            if (map.find(type) == map.end())
                map[type] = {};
            map[type].push_back((size_t)(element.get()));
        });
        std::stringstream ss;
        for (auto iter = map.begin(); iter != map.end(); iter++)
        {
            ss << font_wrapper(BOLDGREEN, iter->first) << "[ ";
            for (auto i : iter->second)
                ss << i << " ";
            ss << "]  ";
        }
        debug_print(ss.rdbuf());
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
