#pragma once

#include "async.h"
#include "../basic/lock.h"

class File : public virtual Object, public EventLoopGetterMixin, public EventLoop::WithHandleMixin
{
public:
    static ref<File> fromPath(ref<String> path, option<EventLoopGetterMixin> getter = nullptr);
    File(ref<String> path, option<EventLoopGetterMixin> getter = nullptr);
    virtual ~File();

    virtual ref<Future<bool>> exists();
    virtual ref<Future<int>> remove();
    virtual ref<Future<long long>> size(); // unit: byte

    virtual bool existsSync();
    virtual int removeSync();
    virtual long long sizeSync(); // unit: byte

    // write
    virtual ref<Future<int>> append(ref<String> str);
    virtual ref<Future<int>> overwrite(ref<String> str);
    virtual ref<Future<int>> clear();

    // read
    virtual ref<Future<ref<String>>> read();
    virtual ref<Stream<ref<String>>> readAsStream(size_t segmentationLength);
    virtual ref<Stream<ref<String>>> readWordAsStream();
    virtual ref<Stream<ref<String>>> readLineAsStream();

    virtual void dispose();

    ref<EventLoop> eventLoop() override;

protected:
    ref<EventLoop> _loop;
    ref<String> _path;
    ref<Lock> _lock;

    std::atomic_bool _isDisposed;

public:
    const ref<String> &path = _path;
};

inline ref<EventLoop> File::eventLoop() { return _loop; }
