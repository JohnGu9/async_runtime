#include <string>
#include <iostream>
#include <sstream>
#include "async_runtime.h"

class MainActivity : public StatelessWidget
{
public:
    virtual Object::Ref<Widget> build(Object::Ref<BuildContext> context)
    {
        return Object::create<LeafWidget>();
    }
};

int main(int argc, char **args)
{
    runApp(Object::create<MainActivity>());
    return EXIT_SUCCESS;
}
