#pragma once

#include <iostream>
#include "async_runtime.h"

class MainActivity : public StatelessWidget
{
public:
    virtual Object::Ref<Widget> build(Object::Ref<BuildContext> context) override
    {
        std::cout << "MainActivity::build" << std::endl;
        return Object::create<LeafWidget>();
    }
};