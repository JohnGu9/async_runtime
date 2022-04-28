#pragma once

#include "../basic/duration.h"
#include "../basic/not_implemented_error.h"

#include "async.h"
#include <fcntl.h>

class File : public virtual Object, public EventLoopGetterMixin
{
public:
    // Checkout your system's [fcntl.h] available flags and mode
    // Unix: https://man7.org/linux/man-pages/man2/open.2.html
    // Windows: https://docs.microsoft.com/en-us/cpp/c-runtime-library/file-constants?view=msvc-170
    using OpenFlags = int;
    using OpenMode = int;

    struct Stat : virtual Object
    {
        uint64_t st_dev;
        uint64_t st_mode;
        uint64_t st_nlink;
        uint64_t st_uid;
        uint64_t st_gid;
        uint64_t st_rdev;
        uint64_t st_ino;
        uint64_t st_size;
        uint64_t st_blksize;
        uint64_t st_blocks;
        uint64_t st_flags;
        uint64_t st_gen;
        Duration st_atim = 0;
        Duration st_mtim = 0;
        Duration st_ctim = 0;
        Duration st_birthtim = 0;

        void toStringStream(std::ostream &ss) override;
    };

    static ref<Future<ref<File>>> fromPath(ref<String> path, OpenFlags flags, OpenMode mode, option<EventLoopGetterMixin> getter = nullptr);
    virtual ~File() {}

    virtual int openCode() { throw NotImplementedError(); }
    virtual int flags() { throw NotImplementedError(); }
    virtual int mode() { throw NotImplementedError(); }
    virtual ref<Future<ref<Stat>>> stat() { throw NotImplementedError(); }

    virtual ref<Future<int>> write(ref<String> str) { throw NotImplementedError(); };
    virtual ref<Future<int>> truncate(int64_t offset) { throw NotImplementedError(); };

    virtual ref<Future<ref<String>>> read() { throw NotImplementedError(); };
    virtual ref<Stream<ref<String>>> readAsStream(size_t segmentationLength) { throw NotImplementedError(); };

    virtual bool isClosed() { return true; }
    virtual ref<Future<int>> close()
    {
        return Future<int>::async([]
                                  { return 0; },
                                  _loop);
    }

    ref<EventLoop> eventLoop() override { return _loop; }

protected:
    ref<EventLoop> _loop;
    ref<String> _path;

    File(ref<String> path, option<EventLoopGetterMixin> getter = nullptr)
        : _loop(ensureEventLoop(getter)), _path(path) {}

public:
    const ref<String> &path = _path;

    class _File;
    class Error;
};

class File::Error : public File
{
    using super = File;

public:
    const int code;
    Error(ref<String> path, const int code, option<EventLoopGetterMixin> getter = nullptr)
        : super(path, getter), code(code) {}

    int openCode() override { return code; }
    virtual int error() { return code; }
};