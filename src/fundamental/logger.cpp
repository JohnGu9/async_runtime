#include "async_runtime/fundamental/logger.h"
#include "async_runtime/fundamental/scheduler.h"
#include "async_runtime/fundamental/file.h"

class _StdoutLoggerHandler : public LoggerHandler
{
public:
    _StdoutLoggerHandler(State<StatefulWidget> *state) : LoggerHandler(state) { assert(state); }
    std::future<bool> write(String str) override
    {
        return this->_callbackHandler->post([str] {
            std::cout << "[" << BOLDGREEN << "INFO " << RESET << "] " << str;
            return true;
        });
    }

    std::future<bool> writeLine(String str) override
    {
        return this->_callbackHandler->post([str] { info_print(str); return true; });
    }
};

class _FileLoggerHandler : public LoggerHandler
{
    Object::Ref<File> _file;

public:
    _FileLoggerHandler(State<StatefulWidget> *state, String path)
        : _file(File::fromPath(state, path)), LoggerHandler(nullptr) {}

    std::future<bool> write(String str) override
    {

        return std::async([] { return true; });
    }

    std::future<bool> writeLine(String str) override
    {
        return std::async([] { return true; });
    }

    void dispose() override
    {
        this->_file->dispose();
        LoggerHandler::dispose();
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
    Object::Ref<State<StatefulWidget>> createState() override;
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
                this->_handler = Object::create<_FileLoggerHandler>(this, this->getWidget()->_path);
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
            this->_handler = Object::create<_FileLoggerHandler>(this, this->getWidget()->_path);

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

Object::Ref<State<StatefulWidget>> _Logger::createState()
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
void _StdoutLoggerState::initState()
{
    super::initState();
    this->_handler = Object::create<_StdoutLoggerHandler>(this);
}

void _StdoutLoggerState::dispose()
{
    this->_handler->dispose();
    super::dispose();
}

Object::Ref<Widget> _StdoutLoggerState::build(Object::Ref<BuildContext>)
{
    return Object::create<_StdoutLoggerInheritedWidget>(
        this->getWidget()->child,
        this->_handler);
}

Logger::Handler StdoutLogger::of(Object::Ref<BuildContext> context)
{
    return context->dependOnInheritedWidgetOfExactType<_StdoutLoggerInheritedWidget>()->_handler;
}

StdoutLogger::StdoutLogger(Object::Ref<Widget> child, Object::Ref<Key> key)
    : child(child), StatefulWidget(key) { assert(child); }

Object::Ref<State<StatefulWidget>> StdoutLogger::createState()
{
    return Object::create<_StdoutLoggerState>();
}

class _LoggerBlockerState : public State<LoggerBlocker>
{
    struct _Blocker : LoggerHandler
    {
        _Blocker(State<StatefulWidget> *state) : LoggerHandler(state) {}
        std::future<bool> write(String str) override
        {
            return std::async([] { return true; });
        }
        std::future<bool> writeLine(String str) override
        {
            return std::async([] { return true; });
        }
    };
    using super = State<LoggerBlocker>;
    Logger::Handler _handler;

    void initState() override
    {
        super::initState();
        _handler = Object::create<_Blocker>(this);
    }

    void dispose() override
    {
        _handler->dispose();
        super::dispose();
    }

    Object::Ref<Widget> build(Object::Ref<BuildContext> context) override
    {
        return Object::create<Logger>(this->getWidget()->child, this->getWidget()->blocking ? _handler : Logger::of(context));
    }
};

Object::Ref<State<StatefulWidget>> LoggerBlocker::createState()
{
    return Object::create<_LoggerBlockerState>();
}