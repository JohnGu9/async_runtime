#pragma once

#include <iostream>
#include <fstream>

#include "dispatcher.h"

class File : public Dispatcher
{
public:
    static Object::Ref<File> fromPath(State<StatefulWidget> *state, String path);
    File(State<StatefulWidget> *state, String path) : _path(path), _isDisposed(false), Dispatcher(state, nullptr, 0)
    {
        static Object::Ref<ThreadPool> sharedThreadPool = Object::create<StaticThreadPool>(1);
        this->_threadPool = sharedThreadPool;
    }

    // write
    virtual std::future<void> append(const String &str);
    virtual std::future<void> append(String &&str);
    virtual std::future<void> append(const String &str, Function<void()> onFinished);
    virtual std::future<void> append(String &&str, Function<void()> onFinished);
    virtual std::future<void> overwrite(const String &str);
    virtual std::future<void> overwrite(String &&str);
    virtual std::future<void> overwrite(const String &str, Function<void()> onFinished);
    virtual std::future<void> overwrite(String &&str, Function<void()> onFinished);
    virtual std::future<void> clear();
    virtual std::future<void> clear(Function<void()> onFinished);

    // read
    virtual std::future<void> read(Function<void(String &)> onRead);
    virtual std::future<void> readWordAsStream(Function<void(String &)> onRead);
    virtual std::future<void> readLineAsStream(Function<void(String &)> onRead);
    virtual std::future<void> readWordAsStream(Function<void(String &)> onRead, Function<void()> onFinished);
    virtual std::future<void> readLineAsStream(Function<void(String &)> onRead, Function<void()> onFinished);

    void dispose() override;

protected:
    String _path;
    std::atomic_bool _isDisposed;
};