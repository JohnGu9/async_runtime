#include <sstream>
#include "async_runtime/fundamental/async.h"
#include "async_runtime/fundamental/logger.h"
#include "async_runtime/fundamental/file.h"

class _Logger : public StatefulWidget
{
    friend class _LoggerState;

public:
    _Logger(ref<Widget> child, option<String> path, option<Key> key)
        : StatefulWidget(key), child(child), path(path) {}
    ref<State<StatefulWidget>> createState() override;
    ref<Widget> child;
    option<String> path;
};

class _LoggerState : public State<_Logger>
{
public:
    class _StdoutLoggerHandler : public LoggerHandler
    {
        ref<State<StatefulWidget>> _state;

    public:
        _StdoutLoggerHandler(State<StatefulWidget> *state) : _state(Object::cast<>(state)) { assert(state); }

        ref<Future<bool>> write(ref<String> str) override
        {
            return async<bool>(this->_state.get(), [str] {
                std::cout << str;
                return true;
            });
        }

        ref<Future<bool>> writeLine(ref<String> str) override
        {
            return async<bool>(this->_state.get(), [str] {
                std::cout << str << std::endl;
                return true;
            });
        }

        void dispose() override {}
    };

    class _FileLoggerHandler : public LoggerHandler
    {
        ref<File> _file;

    public:
        _FileLoggerHandler(State<StatefulWidget> *state, ref<String> path)
            : _file(File::fromPath(state, path)) { _file->clear(); }

        ref<Future<bool>> write(ref<String> str) override
        {
            return this->_file->append(str)->than<bool>([] { return true; });
        }

        ref<Future<bool>> writeLine(ref<String> str) override
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
        _LoggerProxyHandler(Logger::Handler proxyTarget) : _proxyTarget(proxyTarget) {}
        ref<Future<bool>> write(ref<String> str) override
        {
            return this->_proxyTarget->write(str);
        }

        ref<Future<bool>> writeLine(ref<String> str) override
        {
            return this->_proxyTarget->writeLine(str);
        }

        void dispose() override {}
    };

    class _LoggerBlocker : public LoggerHandler
    {
        ref<State<StatefulWidget>> _state;

    public:
        _LoggerBlocker(State<StatefulWidget> *state) : _state(Object::cast<>(state)) {}
        ref<Future<bool>> write(ref<String> str) override
        {
            return Future<bool>::value(this->_state.get(), true);
        }
        ref<Future<bool>> writeLine(ref<String> str) override
        {
            return Future<bool>::value(this->_state.get(), true);
        }

        void dispose() override {}
    };

    using super = State<_Logger>;
    lateref<LoggerHandler> _handler;

    void initState() override
    {
        lateref<String> path;
        if (this->getWidget()->path.isNotNull(path))
        {
            if (path->isEmpty())
                this->_handler = Object::create<_LoggerProxyHandler>(StdoutLogger::of(this->context));
            else
                this->_handler = Object::create<_FileLoggerHandler>(this, path);
        }
        else
            this->_handler = Object::create<_LoggerBlocker>(this);
    }

    void didWidgetUpdated(ref<StatefulWidget> oldWidget) override
    {
        if (oldWidget->covariant<_Logger>()->path != this->getWidget()->path)
        {
            this->_handler->dispose();

            lateref<String> path;
            if (this->getWidget()->path.isNotNull(path))
            {
                if (path->isEmpty())
                    this->_handler = Object::create<_LoggerProxyHandler>(StdoutLogger::of(this->context));
                else
                    this->_handler = Object::create<_FileLoggerHandler>(this, path);
            }
            else
                this->_handler = Object::create<_LoggerBlocker>(this);
        }

        super::didWidgetUpdated(oldWidget);
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
    return context->dependOnInheritedWidgetOfExactType<Logger>().assertNotNull()->_handler;
}

Logger::Logger(ref<Widget> child, Handler handler, option<Key> key)
    : InheritedWidget(child, key), _handler(handler) {}

bool Logger::updateShouldNotify(ref<InheritedWidget> oldWidget)
{
    ref<Logger> old = oldWidget->covariant<Logger>();
    return old->_handler != this->_handler;
}

ref<Widget> Logger::cout(ref<Widget> child, option<Key> key)
{
    return Object::create<_Logger>(child, "", key);
}

ref<Widget> Logger::file(ref<String> path, ref<Widget> child, option<Key> key)
{
    assert(path->isNotEmpty() && "path can't be empty");
    return Object::create<_Logger>(child, path, key);
}

ref<Widget> Logger::block(ref<Widget> child, option<Key> key)
{
    return Object::create<_Logger>(child, nullptr, key);
}

class _StdoutLoggerInheritedWidget : public InheritedWidget
{
    friend StdoutLogger;
    Logger::Handler _handler;

public:
    _StdoutLoggerInheritedWidget(ref<Widget> child, Logger::Handler handler, option<Key> key = nullptr)
        : InheritedWidget(Object::create<Logger>(child, handler), key), _handler(handler) {}

    bool updateShouldNotify(ref<InheritedWidget> oldWidget) override
    {
        ref<_StdoutLoggerInheritedWidget> old = oldWidget->covariant<_StdoutLoggerInheritedWidget>();
        return old->_handler != this->_handler;
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
    return context->dependOnInheritedWidgetOfExactType<_StdoutLoggerInheritedWidget>().assertNotNull()->_handler;
}

StdoutLogger::StdoutLogger(ref<Widget> child, option<Key> key)
    : StatefulWidget(key), child(child) {}
