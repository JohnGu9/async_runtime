#include "async_runtime/widgets/logger_widget.h"
#include "async_runtime/fundamental/async.h"
#include "async_runtime/io/file.h"
#include "async_runtime/widgets/root_widget.h"

static ref<String> endOfLine()
{
    std::stringstream ss;
    ss << std::endl;
    return ss.str();
}

class Logger::_Wrapper : public StatelessWidget
{
public:
    _Wrapper(ref<Widget> child, option<File> file, bool block, option<Key> key)
        : StatelessWidget(key), child(child), file(file), block(block) {}
    ref<Widget> child;
    option<File> file;
    bool block;
    ref<Widget> build(ref<BuildContext> context) override;
};

class Logger::_Logger : public StatefulWidget
{
public:
    class _State;

    _Logger(ref<Widget> child, ref<LoggerHandler> cout, option<File> file, bool block, option<Key> key)
        : StatefulWidget(key), child(child), cout(cout), file(file), block(block) {}
    ref<State<StatefulWidget>> createState() override;
    ref<Widget> child;
    ref<LoggerHandler> cout;
    option<File> file;
    bool block;
};

class Logger::_Logger::_State : public State<Logger::_Logger>
{
public:
    class _FileLoggerHandler : public LoggerHandler
    {
        ref<File> _file;

    public:
        _FileLoggerHandler(ref<File> file)
            : _file(file)
        {
        }

        ref<Future<bool>> write(ref<String> str) override
        {
            return _file->write(str)->then<bool>([](const int &code)
                                                 { return code == 0; });
        }

        ref<Future<bool>> writeLine(ref<String> str) override
        {
            static finalref<String> endl = endOfLine();
            return _file->writeAll({str, endl})->then<bool>([](const int &code)
                                                            { return code == 0; });
        }

        void dispose() override
        {
        }
    };

    class _LoggerBlocker : public LoggerHandler
    {
        ref<Future<bool>> _future;

    public:
        _LoggerBlocker(ref<EventLoopGetterMixin> getter)
            : _future(Future<bool>::value(false, getter)) {}

        ref<Future<bool>> write(ref<String> str) override
        {
            return _future;
        }
        ref<Future<bool>> writeLine(ref<String> str) override
        {
            return _future;
        }

        void dispose() override {}
    };

    using super = State<Logger::_Logger>;
    lateref<LoggerHandler> _handler;

    void updateHandler()
    {
        auto file = widget->file;
        if_not_null(file)
        {
            this->_handler = Object::create<_FileLoggerHandler>(file);
        }
        else_if(widget->block)
        {
            this->_handler = Object::create<_LoggerBlocker>(self());
        }
        else_end()
        {
            this->_handler = widget->cout;
        }
        end_if();
    }

    void initState() override
    {
        super::initState();
        updateHandler();
    }

    void didWidgetUpdated(ref<_Logger> oldWidget) override
    {
        super::didWidgetUpdated(oldWidget);
        if (oldWidget->file != widget->file)
        {
            this->_handler->dispose();
            this->updateHandler();
        }
        else if (widget->file == nullptr && oldWidget->block != widget->block)
        {
            this->_handler->dispose();
            this->updateHandler();
        }
    }

    void dispose() override
    {
        this->_handler->dispose();
        super::dispose();
    }

    ref<Widget> build(ref<BuildContext>) override
    {
        return Object::create<Logger>(widget->child, _handler);
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

ref<Widget> Logger::_Wrapper::build(ref<BuildContext> context)
{
    return Object::create<_Logger>(child, RootWidget::of(context)->cout, file, block, key);
}

ref<Widget> Logger::cout(ref<Widget> child, option<Key> key)
{
    return Object::create<_Wrapper>(child, nullptr, false, key);
}

ref<Widget> Logger::file(ref<Widget> child, ref<File> file, option<Key> key)
{
    return Object::create<_Wrapper>(child, file, false, key);
}

ref<Widget> Logger::block(ref<Widget> child, option<Key> key)
{
    return Object::create<_Wrapper>(child, nullptr, true, key);
}
