#pragma once

#include "../elements/element.h"

class Notification : public virtual Object
{
public:
    virtual void dispatch(ref<BuildContext> context);
};