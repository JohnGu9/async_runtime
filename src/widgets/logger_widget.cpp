#include "async_runtime/widgets/logger_widget.h"
#include "async_runtime/fundamental/async.h"
#include "async_runtime/fundamental/file.h"
#include "async_runtime/fundamental/logger.h"
#include "async_runtime/widgets/root_widget.h"
#include <sstream>

static ref<String> endOfLine()
{
    std::stringstream ss;
    ss << std::endl;
    return ss.str();
}

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
    static bool checkFileAndWrite(ref<File> file, ref<String> str)
    {
        lateref<File::Error> error;
        if (file->cast<File::Error>().isNotNull(error))
        {
            return false;
        }
        file->write(str);
        return true;
    }

    static bool checkFileAndWriteLine(ref<File> file, ref<String> str)
    {
        static finalref<String> endl = endOfLine();
        lateref<File::Error> error;
        if (file->cast<File::Error>().isNotNull(error))
        {
            return false;
        }
        file->writeAll({str, endl});
        return true;
    }

public:
    class _FileLoggerHandler : public LoggerHandler
    {
        ref<Future<ref<File>>> _file;

    public:
        _FileLoggerHandler(ref<String> path, Function<void(ref<File::Error>)> onError, ref<EventLoopGetterMixin> getter)
            : _file(File::fromPath(path, O_WRONLY | O_APPEND | O_CREAT, S_IRUSR | S_IWUSR, getter))
        {
            _file->onCompleted([onError](ref<File> file) //
                               {                         //
                                   lateref<File::Error> error;
                                   if (file->cast<File::Error>().isNotNull(error))
                                       onError(error);
                               });
        }

        ref<Future<bool>> write(ref<String> str) override
        {
            return _file->then<bool>([str](ref<File> file)
                                     { return checkFileAndWrite(file, str); });
        }

        ref<Future<bool>> writeLine(ref<String> str) override
        {
            static finalref<String> endl = endOfLine();
            return _file->then<bool>([str](ref<File> file)
                                     { return checkFileAndWriteLine(file, str); });
        }

        void dispose() override
        {
            _file->then<int>([](ref<File> file)
                             { return file->close(); });
        }
    };

    class _LoggerBlocker : public LoggerHandler
    {
        ref<Future<bool>> _future;

    public:
        _LoggerBlocker(ref<EventLoopGetterMixin> getter)
            : _future(Future<bool>::value(false, EventLoopGetterMixin::ensureEventLoop(getter))) {}

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
        lateref<String> path;
        if (this->widget->path.isNotNull(path))
        {
            if (path->isEmpty())
                this->_handler = RootWidget::of(context)->cout;
            else
                this->_handler = Object::create<_FileLoggerHandler>(
                    path,
                    [this, path](ref<File::Error> error) //
                    {                                    //
                        std::stringstream message("");
                        message << "Logger::file open file [" << path << "] failed with code " << error->openCode() << std::endl;
                        Logger::of(context)->writeLine(message.str());
                    },
                    self());
        }
        else
            this->_handler = Object::create<_LoggerBlocker>(self());
    }

    void didDependenceChanged() override
    {
        this->updateHandler();
    }

    void didWidgetUpdated(ref<_Logger> oldWidget) override
    {
        super::didWidgetUpdated(oldWidget);
        if (oldWidget->path != this->widget->path)
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
