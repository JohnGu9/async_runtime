#pragma once

#include <iostream>
#include <fstream>

#include "dispatcher.h"

class File : public Dispatcher
{
public:
    File(State<StatefulWidget> *state, String path) : _path(path), _isDisposed(false), Dispatcher(state) {}

    // write
    virtual std::future<void> append(const String &str);
    virtual std::future<void> append(String &&str);
    virtual std::future<void> overwrite(const String &str);
    virtual std::future<void> overwrite(String &&str);
    virtual std::future<void> clear();

    // read
    virtual std::future<void> read(Fn<void(String &)> onRead);
    virtual std::future<void> readWordAsStream(Fn<void(String &)> onRead);
    virtual std::future<void> readLineAsStream(Fn<void(String &)> onRead);

    void dispose() override;

protected:
    String _path;
    std::atomic_bool _isDisposed;
};