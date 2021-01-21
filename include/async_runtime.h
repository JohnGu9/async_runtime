#pragma once

// export library
#include "async_runtime/widgets/widget.h"
#include "async_runtime/widgets/leaf_widget.h"
#include "async_runtime/widgets/stateless_widget.h"
#include "async_runtime/widgets/stateful_widget.h"

#include "async_runtime/widgets/multi_child_widget.h"
#include "async_runtime/widgets/inherited_widget.h"
#include "async_runtime/widgets/notification_listener.h"
#include "async_runtime/widgets/builder.h"
#include "async_runtime/widgets/value_listenable_builder.h"
#include "async_runtime/widgets/root_inherited_widget.h"

#include "async_runtime/fundamental/timer.h"
#include "async_runtime/fundamental/logger.h"
#include "async_runtime/fundamental/file.h"

void runApp(Object::Ref<Widget> widget);
