#pragma once

// export library
#include "async_runtime/widgets/widget.h"
#include "async_runtime/widgets/leaf_widget.h"
#include "async_runtime/widgets/stateless_widget.h"
#include "async_runtime/widgets/stateful_widget.h"
#include "async_runtime/widgets/multi_child_widget.h"
#include "async_runtime/widgets/named_multi_child_widget.h"
#include "async_runtime/widgets/inherited_widget.h"
#include "async_runtime/widgets/notification_listener.h"
#include "async_runtime/widgets/builder.h"
#include "async_runtime/widgets/value_listenable_builder.h"
#include "async_runtime/widgets/process.h"
#include "async_runtime/widgets/future_builder.h"

#include "async_runtime/fundamental/async.h"
#include "async_runtime/fundamental/timer.h"
#include "async_runtime/fundamental/logger.h"
#include "async_runtime/fundamental/file.h"
#include "async_runtime/fundamental/scheduler.h"

int runApp(ref<Widget> widget);
