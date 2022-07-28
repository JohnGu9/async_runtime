#include "async_runtime/widgets/stateless_widget.h"
#include "async_runtime/elements/stateless_element.h"

StatelessWidget::StatelessWidget(option<Key> key) : Widget(key) {}

ref<Element> StatelessWidget::createElement() { return Object::create<StatelessElement>(self()); }
