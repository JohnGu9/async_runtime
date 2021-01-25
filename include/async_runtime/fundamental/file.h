#pragma once

#include <iostream>
#include <fstream>
#include "../basic/state.h"
#include "../basic/function.h"
#include "async.h"

class File : public Object
{
public:
    static Object::Ref<File> fromPath(State<StatefulWidget> *state, String path);
    File(State<StatefulWidget> *state, String path);
    virtual ~File();

    // write
    virtual Object::Ref<Future<void>> append(const String &str);
    virtual Object::Ref<Future<void>> append(String &&str);
    virtual Object::Ref<Future<void>> overwrite(const String &str);
    virtual Object::Ref<Future<void>> overwrite(String &&str);
    virtual Object::Ref<Future<void>> clear();

    // read
    virtual Object::Ref<Future<String>> read();
    virtual Object::Ref<Stream<String>> readWordAsStream();
    virtual Object::Ref<Stream<String>> readLineAsStream();

    virtual void dispose();

protected:
    String _path;
    Object::Ref<State<StatefulWidget>> _state;
    Object::Ref<ThreadPool> _threadPool;
    std::atomic_bool _isDisposed;
};