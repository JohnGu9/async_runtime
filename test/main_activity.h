#pragma once

#include "periodic_output.h"
#include "notification_test.h"

class MainActivity : public StatelessWidget
{
    Object::Ref<Widget> build(Object::Ref<BuildContext> context) override
    {
        static Object::List<Object::Ref<Widget>> children = {
            Object::create<PeriodicOutput>(LeafWidget::factory(), Logger::of(context)), // PeriodicOutput
            Object::create<NotificationTest>(),                                         // NotificationTest
        };
        return Object::create<MultiChildWidget>(children);
    }
};
