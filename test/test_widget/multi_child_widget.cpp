#include "async_runtime.h"

struct MyWidget : public StatefulWidget
{
    Object::Ref<StatefulWidget::State> createState() override;
};

struct _MyWidgetState : public State<MyWidget>
{
    using super = StatefulWidget::State;

    static Object::Ref<Widget> onChildBuild(Object::Ref<BuildContext> context)
    {
        debug_print("onChildBuild");
        return LeafWidget::factory();
    };

    Object::List<Object::Ref<Widget>> children;

    void initState() override
    {
        super::initState();
        this->children = {
            Object::create<Builder>(onChildBuild),
            Object::create<Builder>(onChildBuild)
        };
    }

    void dispose() override
    {
        this->children.clear();
        super::dispose();
    }

    void addChild()
    {
        auto self = Object::self(this);
        this->setState([self]() {
            self->children.push_back(Object::create<Builder>(onChildBuild));
        });
    }

    Object::Ref<Widget> build(Object::Ref<BuildContext> context) override
    {
        return Object::create<MultiChildWidget>(this->children);
    }
};

inline Object::Ref<StatefulWidget::State> MyWidget::createState()
{
    return Object::create<_MyWidgetState>();
}

int main()
{
    runApp(Object::create<MyWidget>());
    return EXIT_SUCCESS;
}