#include <sstream>
#include "async_runtime/fundamental/async.h"
#include "async_runtime/fundamental/logger.h"
#include "async_runtime/fundamental/file.h"
#include "async_runtime/widgets/root_widget.h"
#include "async_runtime/widgets/logger_widget.h"

class Logger::_Logger : public StatefulWidget
{
public:
    class _State;

    _Logger(ref<Widget> child, option<String> path, option<Key> key)
        : StatefulWidget(key), child(child), path(path) {}
    ref<State<StatefulWidget>> createState() override;
    ref<Widget> child;
    option<String> path;
};

class Logger::_Logger::_State : public State<Logger::_Logger>
{
    class _InvalidState : public State<StatefulWidget>
    {
        ref<Widget> build(ref<BuildContext>) final { throw std::runtime_error("AsyncRuntime Internal Error"); }
    };

    static ref<State<StatefulWidget>> _invalidState()
    {
        static finalref<State<StatefulWidget>> singleton = Object::create<_InvalidState>();
        return singleton;
    }

public:
    class _StdoutLoggerHandler : public LoggerHandler
    {
        ref<State<StatefulWidget>> _state;

    public:
        _StdoutLoggerHandler(ref<State<StatefulWidget>> state) : _state(state) {}

        ref<Future<bool>> write(ref<String> str) override
        {
            return Future<bool>::async([str]
                               {
                std::cout << str;
                return true; },
                               this->_state);
        }

        ref<Future<bool>> writeLine(ref<String> str) override
        {
            return Future<bool>::async([str]
                               {
                std::cout << str << std::endl;
                return true; },
                               this->_state);
        }

        void dispose() override
        {
            _state = _invalidState();
        }
    };

    class _FileLoggerHandler : public LoggerHandler
    {
        ref<File> _file;

    public:
        _FileLoggerHandler(ref<State<StatefulWidget>> state, ref<String> path)
            : _file(File::fromPath(path, state)) { _file->clear(); }

        ref<Future<bool>> write(ref<String> str) override
        {
            return this->_file->append(str)->then<bool>([](const int &)
                                                        { return true; });
        }

        ref<Future<bool>> writeLine(ref<String> str) override
        {
            return this->_file->append(str + '\n')->then<bool>([](const int &)
                                                               { return true; });
        }

        void dispose() override
        {
            this->_file->dispose();
        }
    };

    class _LoggerBlocker : public LoggerHandler
    {
        ref<State<StatefulWidget>> _state;

    public:
        _LoggerBlocker(ref<State<StatefulWidget>> state) : _state(state) {}
        ref<Future<bool>> write(ref<String> str) override
        {
            return Future<bool>::value(true, this->_state);
        }
        ref<Future<bool>> writeLine(ref<String> str) override
        {
            return Future<bool>::value(true, this->_state);
        }

        void dispose() override
        {
            _state = _invalidState();
        }
    };

    using super = State<Logger::_Logger>;
    lateref<LoggerHandler> _handler;

    void initState() override
    {
        super::initState();
        lateref<String> path;
        if (this->widget->path.isNotNull(path))
        {
            if (path->isEmpty())
                this->_handler = RootWidget::of(context)->cout;
            else
                this->_handler = Object::create<_FileLoggerHandler>(self(), path);
        }
        else
            this->_handler = Object::create<_LoggerBlocker>(self());
    }

    void didWidgetUpdated(ref<_Logger> oldWidget) override
    {
        super::didWidgetUpdated(oldWidget);
        if (oldWidget->path != this->widget->path)
        {
            this->_handler->dispose();

            lateref<String> path;
            if (this->widget->path.isNotNull(path))
            {
                if (path->isEmpty())
                    this->_handler = RootWidget::of(context)->cout;
                else
                    this->_handler = Object::create<_FileLoggerHandler>(self(), path);
            }
            else
                this->_handler = Object::create<_LoggerBlocker>(self());
        }
    }

    void dispose() override
    {
        this->_handler->dispose();
        super::dispose();
    }

    ref<Widget> build(ref<BuildContext>) override
    {
        return Object::create<Logger>(this->widget->child, _handler);
    }
};

ref<State<StatefulWidget>> Logger::_Logger::createState()
{
    return Object::create<_State>();
}

ref<LoggerHandler> Logger::of(ref<BuildContext> context)
{
    return context->dependOnInheritedWidgetOfExactType<Logger>().assertNotNull()->_handler;
}

Logger::Logger(ref<Widget> child, ref<LoggerHandler> handler, option<Key> key)
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
