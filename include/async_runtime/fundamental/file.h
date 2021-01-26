#pragma once

// #include <shared_mutex>
#include "async.h"
#include "../widgets/state.h"

class File : public Object
{
    static Object::Ref<ThreadPool> sharedThreadPool();

public:
    static Object::Ref<File> fromPath(State<StatefulWidget> *state, String path, size_t threads = 0);
    File(State<StatefulWidget> *state, String path, size_t threads = 0 /* if threads == 0, use the shared thread pool*/);
    virtual ~File();

    // write
    virtual Object::Ref<Future<void>> append(String str);
    virtual Object::Ref<Future<void>> overwrite(String str);
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
    // std::shared_mutex _mutex;
};