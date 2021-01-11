#include "async_runtime.h"

struct MyWidget : public StatelessWidget
{
    Object::Ref<Widget> build(Object::Ref<BuildContext> context) override
    {
        return LeafWidget::factory();
    }
};

int main()
{
    runApp(Object::create<MyWidget>());
    return EXIT_SUCCESS;
}