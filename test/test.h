#pragma once

#include "async_runtime.h"

class MainActivity : public StatelessWidget
{
public:
    virtual Object::Ref<Widget> build(Object::Ref<BuildContext> context)
    {
        return Object::create<LeafWidget>();
    }
};