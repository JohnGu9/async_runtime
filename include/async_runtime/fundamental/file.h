#pragma once

// #include <shared_mutex>
#include "async.h"
#include "../widgets/state.h"
#include "../basic/lock.h"

class File : public Object
{
    static Object::Ref<ThreadPool> sharedThreadPool();

public:
    static Object::Ref<File> fromPath(State<StatefulWidget> *state, String path, size_t threads = 0);
    File(State<StatefulWidget> *state, String path, size_t threads = 0 /* if threads == 0, use the shared thread pool*/);
    virtual ~File();

    virtual Object::Ref<Future<bool>> exists();
    virtual Object::Ref<Future<int>> remove();
    virtual Object::Ref<Future<long long>> size(); // unit: byte

    virtual bool existsSync();
    virtual int removeSync();
    virtual long long sizeSync();

    // write
    virtual Object::Ref<Future<void>> append(String str);
    virtual Object::Ref<Future<void>> overwrite(String str);
    virtual Object::Ref<Future<void>> clear();

    // read
    virtual Object::Ref<Future<String>> read();
    virtual Object::Ref<Stream<String>> readAsStream(size_t);
    virtual Object::Ref<Stream<String>> readWordAsStream();
    virtual Object::Ref<Stream<String>> readLineAsStream();

    virtual void dispose();

protected:
    String _path;
    Object::Ref<State<StatefulWidget>> _state;
    Object::Ref<ThreadPool> _threadPool;
    std::atomic_bool _isDisposed;
    Object::Ref<Lock> _lock;

public:
    const String &path = _path;
};