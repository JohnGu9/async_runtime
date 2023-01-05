#pragma once

/**
 * @file file.h
 * @brief
 *
 * io folder for more advanced io-components
 *
 */

#include "../fundamental/async.h"
#include "../utilities/duration.h"
#include <fcntl.h>

class File : public virtual Object, public EventLoopGetterMixin
{

public:
    /**
     * @brief Checkout your system's [fcntl.h] available flags and mode
     * Unix: https://man7.org/linux/man-pages/man2/open.2.html
     * Windows: https://docs.microsoft.com/en-us/cpp/c-runtime-library/file-constants?view=msvc-170
     */
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

        /* not available yet */
        Duration st_atim = 0;
        Duration st_mtim = 0;
        Duration st_ctim = 0;
        Duration st_birthtim = 0;

        void toStringStream(std::ostream &ss) override;
    };

    /**
     * @brief Create a file object from given path string.
     * Checkout your system's [fcntl.h] available flags and mode.
     * Unix: https://man7.org/linux/man-pages/man2/open.2.html
     * Windows: https://docs.microsoft.com/en-us/cpp/c-runtime-library/file-constants?view=msvc-170
     *
     * @param path file's path (absolute or relative)
     * @param flags depend on system fcntl.h define
     * @param mode depend on system fcntl.h define
     * @param getter loop
     * @return ref<Future<ref<File>>>
     */
    static ref<Future<ref<File>>> fromPath(ref<String> path, OpenFlags flags, OpenMode mode, option<EventLoopGetterMixin> getter = nullptr);
    static ref<Future<int>> unlink(ref<String> path, option<EventLoopGetterMixin> getter = nullptr);

    virtual int error() noexcept { return code; }
    virtual int flags() = 0;
    virtual int mode() = 0;
    virtual ref<Future<ref<Stat>>> stat() = 0;

    virtual ref<Future<int>> write(ref<String> str) = 0;
    virtual ref<Future<int>> writeAll(ref<List<ref<String>>> str) = 0;
    virtual ref<Future<int>> truncate(int64_t offset) = 0;

    virtual ref<Future<ref<String>>> read() = 0;
    virtual ref<Stream<ref<String>>> readAsStream(size_t segmentationLength) = 0;

    virtual bool isClosed() noexcept = 0;
    virtual ref<Future<int>> close() = 0;

    ref<EventLoop> eventLoop() override { return _loop; }

protected:
    ref<EventLoop> _loop;
    const int code;
    ref<String> _path;

    File(ref<String> path, const int code, option<EventLoopGetterMixin> getter = nullptr)
        : _loop(ensureEventLoop(getter)), code(code), _path(path) {}

public:
    const ref<String> &path = _path;

    class Success;
    class Error;
};

class File::Error : public File
{
    using super = File;

public:
    Error(ref<String> path, const int code, option<EventLoopGetterMixin> getter = nullptr)
        : super(path, code, getter) {}

    int flags() override;
    int mode() override;
    ref<Future<ref<Stat>>> stat() override;

    ref<Future<int>> write(ref<String> str) override;
    ref<Future<int>> writeAll(ref<List<ref<String>>> str) override;
    ref<Future<int>> truncate(int64_t offset) override;

    ref<Future<ref<String>>> read() override;
    ref<Stream<ref<String>>> readAsStream(size_t segmentationLength) override;

    bool isClosed() noexcept override;
    ref<Future<int>> close() override;
};
