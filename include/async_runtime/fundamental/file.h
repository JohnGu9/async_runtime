#pragma once

#include "async.h"
#include "disposable.h"
#include "dispatcher.h"
#include "../widgets/state.h"
#include "../basic/lock.h"

class File : public AsyncDispatcher
{
public:
    static ref<File> fromPath(ref<State<StatefulWidget>> state, ref<String> path, size_t threads = 0);
    File(ref<State<StatefulWidget>> state, ref<String> path, size_t threads = 0 /* if threads == 0, use the shared thread pool*/);
    virtual ~File();

    virtual ref<Future<bool>> exists();
    virtual ref<Future<int>> remove();
    virtual ref<Future<long long>> size(); // unit: byte

    virtual bool existsSync();
    virtual int removeSync();
    virtual long long sizeSync(); // unit: byte

    // write
    virtual ref<Future<void>> append(ref<String> str);
    virtual ref<Future<void>> overwrite(ref<String> str);
    virtual ref<Future<void>> clear();

    // read
    virtual ref<Future<ref<String>>> read();
    virtual ref<Stream<ref<String>>> readAsStream(size_t segmentationLength);
    virtual ref<Stream<ref<String>>> readWordAsStream();
    virtual ref<Stream<ref<String>>> readLineAsStream();

    void dispose() override;

protected:
    ref<String> _path;
    ref<Lock> _lock;

    std::atomic_bool _isDisposed;

public:
    const ref<String> &path = _path;
};
