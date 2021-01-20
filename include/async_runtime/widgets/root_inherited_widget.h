#include "inherited_widget.h"

class RootInheritedWidget : public InheritedWidget
{
public:
    static Object::Ref<RootInheritedWidget> of(Object::Ref<BuildContext> context);
    RootInheritedWidget(Object::Ref<Widget> child, Object::Ref<RootElement> root, Object::Ref<Key> key = nullptr)
        : _root(root), InheritedWidget(child, key) {}

    bool updateShouldNotify(Object::Ref<InheritedWidget> oldWidget) override;
    virtual void requestExit();

protected:
    Object::WeakRef<RootElement> _root;
};