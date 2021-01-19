#include "framework/fundamental/logger.h"
#include "framework/fundamental/scheduler.h"

class _StdoutLoggerHandler : public LoggerHandler
{
public:
    _StdoutLoggerHandler(Scheduler::Handler sync) : LoggerHandler(sync) {}
    std::future<bool> write(String str) override
    {
        return this->sync->post([str] {
            std::cout << "[" << BOLDGREEN << "INFO " << RESET << "] " << str;
            return true;
        });
    }

    std::future<bool> writeLine(String str) override
    {
        return this->sync->post([str] { info_print(str);return true; });
    }
};

class _FileLoggerHandler : public LoggerHandler
{
    String _path;
    Logger::Handler _proxyTarget;

public:
    _FileLoggerHandler(Logger::Handler proxyTarget, String path)
        : _proxyTarget(proxyTarget), _path(path), LoggerHandler(nullptr) { assert(!this->_path.empty()); }

    std::future<bool> write(String str) override
    {
        return this->_proxyTarget->write(" [File out not finish yet] " + str);
    }

    std::future<bool> writeLine(String str) override
    {
        return this->_proxyTarget->writeLine(" [File out not finish yet] " + str);
    }
};

class _LoggerProxyHandler : public LoggerHandler
{
    Logger::Handler _proxyTarget;

public:
    _LoggerProxyHandler(Logger::Handler proxyTarget) : LoggerHandler(nullptr) {}
    std::future<bool> write(String str) override
    {
        return this->_proxyTarget->write(" [File out not finish yet] " + str);
    }

    std::future<bool> writeLine(String str) override
    {
        return this->_proxyTarget->writeLine(str);
    }
};

class __LoggerState;
class _Logger : public StatefulWidget
{
    friend __LoggerState;
    Object::Ref<State> createState() override;
    Object::Ref<Widget> _child;
    String _path;

public:
    _Logger(Object::Ref<Widget> child, String path, Object::Ref<Key> key);
};

class __LoggerState : public State<_Logger>
{
    using super = State<_Logger>;
    Logger::Handler _handler;
    Scheduler::Handler _schedulerHandler;

    void didWidgetUpdated(Object::Ref<StatefulWidget> oldWidget) override
    {
        if (Object::Ref<_Logger> old = oldWidget->cast<_Logger>())
        {
            this->_handler->dispose();
            if (this->getWidget()->_path.empty())
                this->_handler = Object::create<_LoggerProxyHandler>(StdoutLogger::of(this->getContext()));
            else
                this->_handler = Object::create<_FileLoggerHandler>(StdoutLogger::of(this->getContext()), this->getWidget()->_path);
        }
        else
        {
            assert(false);
        }

        super::didWidgetUpdated(oldWidget);
    }

    void didDependenceChanged() override
    {
        if (this->_handler != nullptr)
            this->_handler->dispose();
        if (this->getWidget()->_path.empty())
            this->_handler = Object::create<_LoggerProxyHandler>(StdoutLogger::of(this->getContext()));
        else
            this->_handler = Object::create<_FileLoggerHandler>(StdoutLogger::of(this->getContext()), this->getWidget()->_path);

        super::didDependenceChanged();
    }

    void dispose() override
    {
        this->_handler->dispose();
        super::dispose();
    }

    Object::Ref<Widget> build(Object::Ref<BuildContext>) override
    {
        return Object::create<Logger>(this->getWidget()->_child, _handler);
    }
};

Logger::Handler Logger::of(Object::Ref<BuildContext> context)
{
    return context->dependOnInheritedWidgetOfExactType<Logger>()->_handler;
}

Logger::Logger(Object::Ref<Widget> child, Handler handler, Object::Ref<Key> key)
    : _handler(handler), InheritedWidget(child, key) { assert(this->_handler); }

bool Logger::updateShouldNotify(Object::Ref<InheritedWidget> oldWidget)
{
    if (Object::Ref<Logger> old = oldWidget->cast<Logger>())
        return old->_handler != this->_handler;
    else
    {
        assert(false);
        return true;
    }
}

_Logger::_Logger(Object::Ref<Widget> child, String path, Object::Ref<Key> key)
    : _child(child), _path(path), StatefulWidget(key) {}

Object::Ref<StatefulWidget::State> _Logger::createState()
{
    return Object::create<__LoggerState>();
}

Object::Ref<Widget> Logger::stdoutProxy(Object::Ref<Widget> child, Object::Ref<Key> key)
{
    return Object::create<_Logger>(child, "", key);
}

Object::Ref<Widget> Logger::file(Object::Ref<Widget> child, String path, Object::Ref<Key> key)
{
    assert(!path.empty() && "path can't be empty");
    return Object::create<_Logger>(child, path, key);
}

class _StdoutLoggerInheritedWidget : public InheritedWidget
{
    friend StdoutLogger;
    Logger::Handler _handler;

public:
    _StdoutLoggerInheritedWidget(Object::Ref<Widget> child, Logger::Handler handler, Object::Ref<Key> key = nullptr)
        : _handler(handler), InheritedWidget(Object::create<Logger>(child, handler), key) { assert(handler); }

    bool updateShouldNotify(Object::Ref<InheritedWidget> oldWidget) override
    {
        if (Object::Ref<_StdoutLoggerInheritedWidget> old = oldWidget->cast<_StdoutLoggerInheritedWidget>())
            return old->_handler != this->_handler;
        else
        {
            assert(false);
            return true;
        }
    }
};

void _StdoutLoggerState::didDependenceChanged()
{
    assert(this->getContext()->dependOnInheritedWidgetOfExactType<_StdoutLoggerInheritedWidget>() == nullptr && "Duplicate StdoutLogger. ");
    if (this->_handler == nullptr)
        this->_handler = Object::create<_StdoutLoggerHandler>(Scheduler::of(this->getContext()));
    else
    {
        assert(_handler->sync == Scheduler::of(this->getContext()) && "StdoutLogger sync should not change. ");
    }
    super::didDependenceChanged();
}

void _StdoutLoggerState::dispose()
{
    this->_handler->dispose();
    super::dispose();
}

Object::Ref<Widget> _StdoutLoggerState::build(Object::Ref<BuildContext>)
{
    return Object::create<_StdoutLoggerInheritedWidget>(
        this->getWidget()->_child,
        this->_handler);
}

Logger::Handler StdoutLogger::of(Object::Ref<BuildContext> context)
{
    return context->dependOnInheritedWidgetOfExactType<_StdoutLoggerInheritedWidget>()->_handler;
}

StdoutLogger::StdoutLogger(Object::Ref<Widget> child, Object::Ref<Key> key)
    : _child(child), StatefulWidget(key) { assert(_child); }

Object::Ref<StatefulWidget::State> StdoutLogger::createState()
{
    return Object::create<_StdoutLoggerState>();
}

