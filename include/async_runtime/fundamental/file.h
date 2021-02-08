#pragma once

#include "async.h"
#include "disposable.h"
#include "dispatcher.h"
#include "../widgets/state.h"
#include "../basic/lock.h"

class File : public Dispatcher
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
    virtual long long sizeSync(); // unit: byte

    // write
    virtual Object::Ref<Future<void>> append(String str);
    virtual Object::Ref<Future<void>> overwrite(String str);
    virtual Object::Ref<Future<void>> clear();

    // read
    virtual Object::Ref<Future<String>> read();
    virtual Object::Ref<Stream<String>> readAsStream(size_t segmentationLength);
    virtual Object::Ref<Stream<String>> readWordAsStream();
    virtual Object::Ref<Stream<String>> readLineAsStream();

    void dispose() override;

protected:
    String _path;
    std::atomic_bool _isDisposed;

    Object::Ref<State<StatefulWidget>> _state;
    Object::Ref<Lock> _lock;

public:
    const String &path = _path;
};