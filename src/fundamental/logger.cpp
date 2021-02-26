#include <sstream>
#include "async_runtime/fundamental/async.h"
#include "async_runtime/fundamental/logger.h"
#include "async_runtime/fundamental/file.h"

class _Logger : public StatefulWidget
{
    friend class _LoggerState;

public:
    _Logger(ref<Widget> child, String path, ref<Key> key)
        : child(child), path(path), StatefulWidget(key) {}
    ref<State<StatefulWidget>> createState() override;
    ref<Widget> child;
    String path;
};

class _LoggerState : public State<_Logger>
{
public:
    class _StdoutLoggerHandler : public LoggerHandler
    {
    public:
        _StdoutLoggerHandler(State<StatefulWidget> *state) : LoggerHandler(state) { assert(state); }
        ref<Future<bool>> write(String str) override
        {
            return async<bool>(this->_state.get(), [str] {
                std::cout << "[" << BOLDGREEN << "INFO " << RESET << "] " << str;
                return true;
            });
        }

        ref<Future<bool>> writeLine(String str) override
        {
            return async<bool>(this->_state.get(), [str] {
                info_print(str);
                return true;
            });
        }

        void dispose() override {}
    };

    class _FileLoggerHandler : public LoggerHandler
    {
        ref<File> _file;

    public:
        _FileLoggerHandler(State<StatefulWidget> *state, String path)
            : _file(File::fromPath(state, path)), LoggerHandler(state) { _file->clear(); }

        ref<Future<bool>> write(String str) override
        {
            return this->_file->append(str)->than<bool>([] { return true; });
        }

        ref<Future<bool>> writeLine(String str) override
        {
            return this->_file->append(str + '\n')->than<bool>([] { return true; });
        }

        void dispose() override
        {
            this->_file->dispose();
        }
    };

    class _LoggerProxyHandler : public LoggerHandler
    {
        Logger::Handler _proxyTarget;

    public:
        _LoggerProxyHandler(Logger::Handler proxyTarget) : _proxyTarget(proxyTarget), LoggerHandler() {}
        ref<Future<bool>> write(String str) override
        {
            return this->_proxyTarget->write(str);
        }

        ref<Future<bool>> writeLine(String str) override
        {
            return this->_proxyTarget->writeLine(str);
        }

        void dispose() override {}
    };

    class _LoggerBlocker : public LoggerHandler
    {
    public:
        _LoggerBlocker(State<StatefulWidget> *state) : LoggerHandler(state) {}
        ref<Future<bool>> write(String str) override
        {
            return Future<bool>::value(this->_state.get(), true);
        }
        ref<Future<bool>> writeLine(String str) override
        {
            return Future<bool>::value(this->_state.get(), true);
        }

        void dispose() override {}
    };

    using super = State<_Logger>;
    Logger::Handler _handler;

    void didWidgetUpdated(ref<StatefulWidget> oldWidget) override
    {
        if (ref<_Logger> old = oldWidget->cast<_Logger>())
        {
            this->_handler->dispose();

            if (not this->getWidget()->path)
                this->_handler = Object::create<_LoggerBlocker>(this);
            else if (this->getWidget()->path.isEmpty())
                this->_handler = Object::create<_LoggerProxyHandler>(StdoutLogger::of(this->context));
            else
                this->_handler = Object::create<_FileLoggerHandler>(this, this->getWidget()->path);
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

        if (not this->getWidget()->path)
            this->_handler = Object::create<_LoggerBlocker>(this);
        else if (this->getWidget()->path.isEmpty())
            this->_handler = Object::create<_LoggerProxyHandler>(StdoutLogger::of(this->context));
        else
            this->_handler = Object::create<_FileLoggerHandler>(this, this->getWidget()->path);

        super::didDependenceChanged();
    }

    void dispose() override
    {
        this->_handler->dispose();
        super::dispose();
    }

    ref<Widget> build(ref<BuildContext>) override
    {
        return Object::create<Logger>(this->getWidget()->child, _handler);
    }
};

ref<State<StatefulWidget>> _Logger::createState()
{
    return Object::create<_LoggerState>();
}

Logger::Handler Logger::of(ref<BuildContext> context)
{
    return context->dependOnInheritedWidgetOfExactType<Logger>()->_handler;
}

Logger::Logger(ref<Widget> child, Handler handler, ref<Key> key)
    : _handler(handler), InheritedWidget(child, key) { assert(this->_handler); }

bool Logger::updateShouldNotify(ref<InheritedWidget> oldWidget)
{
    if (ref<Logger> old = oldWidget->cast<Logger>())
        return old->_handler != this->_handler;
    else
    {
        assert(false);
        return true;
    }
}

ref<Widget> Logger::stdout(ref<Widget> child, ref<Key> key)
{
    return Object::create<_Logger>(child, "", key);
}

ref<Widget> Logger::file(String path, ref<Widget> child, ref<Key> key)
{
    assert(!path.isEmpty() && "path can't be empty");
    return Object::create<_Logger>(child, path, key);
}

ref<Widget> Logger::block(ref<Widget> child, ref<Key> key)
{
    return Object::create<_Logger>(child, String(), key);
}

class _StdoutLoggerInheritedWidget : public InheritedWidget
{
    friend StdoutLogger;
    Logger::Handler _handler;

public:
    _StdoutLoggerInheritedWidget(ref<Widget> child, Logger::Handler handler, ref<Key> key = nullptr)
        : _handler(handler), InheritedWidget(Object::create<Logger>(child, handler), key) { assert(handler); }

    bool updateShouldNotify(ref<InheritedWidget> oldWidget) override
    {
        if (ref<_StdoutLoggerInheritedWidget> old = oldWidget->cast<_StdoutLoggerInheritedWidget>())
            return old->_handler != this->_handler;
        else
        {
            assert(false);
            return true;
        }
    }
};
void StdoutLoggerState::initState()
{
    super::initState();
    this->_handler = Object::create<_LoggerState::_StdoutLoggerHandler>(this);
}

void StdoutLoggerState::dispose()
{
    this->_handler->dispose();
    super::dispose();
}

ref<Widget> StdoutLoggerState::build(ref<BuildContext>)
{
    return Object::create<_StdoutLoggerInheritedWidget>(
        this->getWidget()->child,
        this->_handler);
}

Logger::Handler StdoutLogger::of(ref<BuildContext> context)
{
    return context->dependOnInheritedWidgetOfExactType<_StdoutLoggerInheritedWidget>()->_handler;
}

StdoutLogger::StdoutLogger(ref<Widget> child, ref<Key> key)
    : child(child), StatefulWidget(key) { assert(child); }
