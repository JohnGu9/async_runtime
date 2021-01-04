#include "../elements/element.h"

class Notification : public virtual Object
{
public:
    virtual void dispatch(Object::Ref<BuildContext> context);
};