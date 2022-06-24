#pragma once

#include "../elements/stateful_element.h"
#include "../fundamental/async.h"
#include "state.h"
#include "widget.h"

class StatefulWidget : public Widget
{
public:
    StatefulWidget(option<Key> key = nullptr) : Widget(key) {}
    virtual ref<State<StatefulWidget>> createState() = 0;

protected:
    ref<Element> createElement() override;
};

template <>
class State<StatefulWidget> : public virtual Object, public EventLoopGetterMixin
{
    friend class StatefulElement;

    template <typename T, typename std::enable_if<std::is_base_of<StatefulWidget, T>::value>::type *>
    friend class State;

    bool _mounted = false;
    ref<EventLoop> _loop;
    lateref<StatefulElement> _element;
    lateref<BuildContext> _context;

    virtual void initState();
    virtual void dispose();

    virtual void didDependenceChanged() = 0;
    virtual void callDidWidgetUpdated(ref<StatefulWidget> oldWidget) = 0;
    virtual ref<Widget> build(ref<BuildContext> context) = 0;

/**
 * @brief If your compiler can't gather the protected member in lambda
 * you can enable this flag [ASYNC_RUNTIME_STATE_API_PUBLIC] to make api public when compile the codes
 *
 */
#ifdef ASYNC_RUNTIME_STATE_API_PUBLIC
public:
#else
protected:
#endif

    const bool &mounted = _mounted;
    const ref<BuildContext> &context = _context;
    void setState(Function<void()> fn);

public:
    State() : _loop(EventLoopGetterMixin::ensureEventLoop(nullptr)) {}
    ~State() { DEBUG_ASSERT(_mounted == false && "[dispose] must to call super::dispose"); }

    ref<EventLoop> eventLoop() override { return this->_loop; }
};

template <typename T, typename std::enable_if<std::is_base_of<StatefulWidget, T>::value>::type *>
class State : public State<StatefulWidget>
{
    using super = State<StatefulWidget>;
    lateref<T> _widget;

#ifdef ASYNC_RUNTIME_STATE_API_PUBLIC
public:
#else
protected:
#endif

    /**
     * @brief mustCallSuper
     * [the hook invoke when widget mounting]
     *
     * @example
     * void initState() override
     * {
     *      super::initState(); // call before your custom actions
     *      ......
     * }
     *
     */
    void initState() override
    {
        super::initState();
        this->_widget = this->_element->_statefulWidget->template covariant<T>();
    }

    /**
     * @brief mustCallSuper
     * [the hook invoke when widget change]
     *
     * @example
     * void didWidgetUpdated(ref<T> oldWidget) override
     * {
     *      super::didWidgetUpdated(oldWidget);
     *      ......
     * }
     *
     */
    virtual void didWidgetUpdated(ref<T> oldWidget) {}

    /**
     * @brief mustCallSuper
     * [the hook invoke when context change or when widget mounting]
     *
     * @example
     * void didDependenceChanged() override
     * {
     *      super::didDependenceChanged();
     *      ......
     * }
     *
     */
    void didDependenceChanged() override {}

    /**
     * @brief  mustCallSuper
     * [the hook invoke when widget unmounting]
     *
     * @example
     * void dispose() override
     * {
     *      ......
     *      super::dispose(); // call after your custom actions
     * }
     *
     */
    void dispose() override
    {
        Object::detach(this->_widget);
        super::dispose();
    }

    finalref<T> &widget = _widget;

private:
    void callDidWidgetUpdated(ref<StatefulWidget> oldWidget) override final
    {
        this->_widget = this->_element->_statefulWidget->template covariant<T>();
        this->didWidgetUpdated(oldWidget->covariant<T>());
    }
};
