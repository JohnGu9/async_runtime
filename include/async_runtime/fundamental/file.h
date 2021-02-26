#pragma once

#include "async.h"
#include "disposable.h"
#include "dispatcher.h"
#include "../widgets/state.h"
#include "../basic/lock.h"

class File : public AsyncDispatcher
{
    static ref<ThreadPool> sharedThreadPool();

public:
    static ref<File> fromPath(State<StatefulWidget> *state, String path, size_t threads = 0);
    File(State<StatefulWidget> *state, String path, size_t threads = 0 /* if threads == 0, use the shared thread pool*/);
    virtual ~File();

    virtual ref<Future<bool>> exists();
    virtual ref<Future<int>> remove();
    virtual ref<Future<long long>> size(); // unit: byte

    virtual bool existsSync();
    virtual int removeSync();
    virtual long long sizeSync(); // unit: byte

    // write
    virtual ref<Future<void>> append(String str);
    virtual ref<Future<void>> overwrite(String str);
    virtual ref<Future<void>> clear();

    // read
    virtual ref<Future<String>> read();
    virtual ref<Stream<String>> readAsStream(size_t segmentationLength);
    virtual ref<Stream<String>> readWordAsStream();
    virtual ref<Stream<String>> readLineAsStream();

    void dispose() override;

protected:
    String _path;
    std::atomic_bool _isDisposed;

    ref<State<StatefulWidget>> _state;
    ref<Lock> _lock;

public:
    const String &path = _path;
};