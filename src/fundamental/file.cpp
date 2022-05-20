#include "async_runtime/fundamental/file.h"

#include <fstream>
#include <iostream>

extern "C"
{
#include <uv.h>
}

void File::Stat::toStringStream(std::ostream &ss)
{
    ss << "File::Stat" << std::endl
       << " st_dev - " << st_dev << std::endl
       << " st_mode - " << st_mode << std::endl
       << " st_nlink - " << st_nlink << std::endl
       << " st_uid - " << st_uid << std::endl
       << " st_gid - " << st_gid << std::endl
       << " st_rdev - " << st_rdev << std::endl
       << " st_ino - " << st_ino << std::endl
       << " st_size - " << st_size << std::endl
       << " st_blocks - " << st_blocks << std::endl
       << " st_flags - " << st_flags << std::endl
       << " st_gen - " << st_gen << std::endl;
}

class File::_File : public File
{
public:
    using super = File;

    uv_loop_t *loop;
    uv_fs_t *openRequest;
    ref<Completer<int>> closed;

    _File(ref<String> path, uv_fs_t *openRequest, option<EventLoopGetterMixin> getter)
        : super(path, static_cast<int>(openRequest->result), getter),
          loop(reinterpret_cast<uv_loop_t *>(_loop->nativeHandle())),
          openRequest(openRequest),
          closed(Object::create<Completer<int>>(getter)) {}
    ~_File() { close(); }
    int flags() override { return openRequest->flags; }

#ifndef _WIN32
    int mode() override
    {
        return openRequest->mode;
    }
#endif

    struct _stat_data
    {
        _stat_data(ref<Completer<ref<Stat>>> completer) : completer(completer) {}
        ref<Completer<ref<Stat>>> completer;
    };

    static void copy(Stat &stat, uv_stat_t &buf)
    {
        stat.st_dev = buf.st_dev;
        stat.st_mode = buf.st_mode;
        stat.st_nlink = buf.st_nlink;
        stat.st_uid = buf.st_uid;
        stat.st_gid = buf.st_gid;
        stat.st_rdev = buf.st_rdev;
        stat.st_ino = buf.st_ino;
        stat.st_size = buf.st_size;
        stat.st_blksize = buf.st_blksize;
        stat.st_blocks = buf.st_blocks;
        stat.st_flags = buf.st_flags;
        stat.st_gen = buf.st_gen;
    }

    static void _on_stat(uv_fs_t *req)
    {
        auto data = reinterpret_cast<_stat_data *>(req->data);
        auto stat = Object::create<Stat>();
        data->completer->complete(stat);
        copy(*stat, req->statbuf);
        uv_fs_req_cleanup(req);
        delete data;
        delete req;
    }
    ref<Future<ref<Stat>>> stat() override
    {
        auto request = new uv_fs_t;
        auto data = new _stat_data(Object::create<Completer<ref<Stat>>>());
        request->data = data;
        uv_fs_fstat(loop, request, static_cast<uv_file>(openRequest->result), _on_stat);
        return data->completer;
    }

    struct _write_data
    {
        _write_data(ref<Completer<int>> completer, ref<String> str) : completer(completer), str(str) {}
        ref<Completer<int>> completer;
        ref<String> str;
    };

    static void _on_write(uv_fs_t *req)
    {
        auto data = reinterpret_cast<_write_data *>(req->data);
        data->completer->complete(static_cast<int>(req->result));
        uv_fs_req_cleanup(req);
        delete data;
        delete req;
    }
    ref<Future<int>> write(ref<String> str) override
    {
        auto request = new uv_fs_t;
        auto data = new _write_data(Object::create<Completer<int>>(_loop), str);
        request->data = data;
        uv_buf_t buffer = uv_buf_init(const_cast<char *>(str->c_str()), static_cast<unsigned int>(str->length()));
        uv_fs_write(loop, request, static_cast<uv_file>(openRequest->result), &buffer, 1, 0, _on_write);
        return data->completer;
    }

    struct _write_all_data
    {
        _write_all_data(ref<Completer<int>> completer, ref<List<ref<String>>> str) : completer(completer), str(str) {}
        ref<Completer<int>> completer;
        ref<List<ref<String>>> str;
    };

    static void _on_write_all(uv_fs_t *req)
    {
        auto data = reinterpret_cast<_write_data *>(req->data);
        data->completer->complete(static_cast<int>(req->result));
        uv_fs_req_cleanup(req);
        delete data;
        delete req;
    }
    ref<Future<int>> writeAll(ref<List<ref<String>>> str) override
    {
        auto request = new uv_fs_t;
        auto data = new _write_all_data{Object::create<Completer<int>>(_loop), str->copy()};
        request->data = data;

        const auto length = str->size();
        uv_buf_t *buffers = new uv_buf_t[length];
        for (auto i = 0; i < length; i++)
        {
            auto &value = str[i];
            buffers[i] = uv_buf_init(const_cast<char *>(value->c_str()), static_cast<unsigned int>(value->length()));
        }
        uv_fs_write(loop, request, static_cast<uv_file>(openRequest->result), buffers, static_cast<unsigned int>(length), -1, _on_write_all);
        delete[] buffers;
        return data->completer;
    };

    struct _truncate_data
    {
        _truncate_data(ref<Completer<int>> completer) : completer(completer) {}
        ref<Completer<int>> completer;
    };

    static void _on_truncate(uv_fs_t *req)
    {
        auto data = reinterpret_cast<_truncate_data *>(req->data);
        data->completer->complete(static_cast<int>(req->result));
        uv_fs_req_cleanup(req);
        delete data;
        delete req;
    }
    ref<Future<int>> truncate(int64_t offset) override
    {
        auto request = new uv_fs_t;
        auto data = new _truncate_data{Object::create<Completer<int>>()};
        request->data = data;
        uv_fs_ftruncate(loop, request, static_cast<uv_file>(openRequest->result), offset, _on_truncate);
        return data->completer;
    }

    struct _read_data
    {
        _read_data(ref<_File> file,
                   lateref<Completer<ref<String>>> completer,
                   lateref<StreamController<ref<String>>> stream,
                   std::stringstream &&ss,
                   uv_buf_t &&buffer) : file(file), completer(completer), stream(stream), ss(std::move(ss)), buffer(std::move(buffer)) {}
        ref<_File> file;
        lateref<Completer<ref<String>>> completer;
        lateref<StreamController<ref<String>>> stream;
        std::stringstream ss;
        uv_buf_t buffer;
    };

    static void _on_read(uv_fs_t *req)
    {
        auto data = reinterpret_cast<_read_data *>(req->data);
        if (req->result <= 0)
        {
            data->completer->complete(data->ss.str());
            uv_fs_req_cleanup(req);
            delete[] data->buffer.base;
            delete data;
            delete req;
        }
        else
        {
            data->ss << data->buffer.base;
            memset(data->buffer.base, 0, data->buffer.len);
            uv_fs_req_cleanup(req);
            uv_fs_read(data->file->loop, req, static_cast<uv_file>(data->file->openRequest->result), &(data->buffer), 1, -1, _on_read);
        }
    }
    ref<Future<ref<String>>> read() override
    {
        auto request = new uv_fs_t;
        auto data = new _read_data{
            self(),
            Object::create<Completer<ref<String>>>(_loop),
            lateref<StreamController<ref<String>>>(),
            std::stringstream(""),
            uv_buf_init(new char[1024], 1023),
        };
        memset(data->buffer.base, 0, 1024);
        request->data = data;
        uv_fs_read(loop, request, static_cast<uv_file>(openRequest->result), &(data->buffer), 1, -1, _on_read);
        return data->completer;
    }

    static void _on_read_stream(uv_fs_t *req)
    {
        auto data = reinterpret_cast<_read_data *>(req->data);
        if (req->result <= 0)
        {
            data->stream->close();
            uv_fs_req_cleanup(req);
            delete[] data->buffer.base;
            delete data;
            delete req;
        }
        else
        {
            data->stream->sink(data->buffer.base);
            memset(data->buffer.base, 0, data->buffer.len);
            uv_fs_req_cleanup(req);
            uv_fs_read(data->file->loop, req, static_cast<uv_file>(data->file->openRequest->result), &(data->buffer), 1, -1, _on_read_stream);
        }
    }
    ref<Stream<ref<String>>> readAsStream(size_t segmentationLength) override
    {
        auto request = new uv_fs_t;
        auto data = new _read_data{
            self(),
            lateref<Completer<ref<String>>>(),
            Object::create<StreamController<ref<String>>>(_loop),
            std::stringstream(""),
            uv_buf_init(new char[segmentationLength + 1], static_cast<unsigned int>(segmentationLength)),
        };
        memset(data->buffer.base, 0, segmentationLength + 1);
        request->data = data;
        uv_fs_read(loop, request, static_cast<uv_file>(openRequest->result), &(data->buffer), 1, -1, _on_read_stream);
        return data->stream;
    }

    bool isClosed() noexcept override { return openRequest == nullptr; }

    static void _on_close(uv_fs_t *req)
    {
        auto closed = reinterpret_cast<ref<Completer<int>> *>(req->data);
        (*closed)->complete(static_cast<int>(req->result));
        uv_fs_req_cleanup(req);
        delete closed;
        delete req;
    }

    ref<Future<int>> close() override
    {
        if (openRequest != nullptr)
        {
            auto request = new uv_fs_t;
            request->data = new ref<Completer<int>>(closed);
            uv_fs_close(loop, request, static_cast<uv_file>(openRequest->result), _on_close);
            delete openRequest;
            openRequest = nullptr;
        }
        return closed;
    }

    struct _open_data
    {
        _open_data(ref<String> path,
                   ref<Completer<ref<File>>> completer)
            : path(path), completer(completer) {}
        ref<String> path;
        ref<Completer<ref<File>>> completer;
    };

    struct _unlink_data
    {
        _unlink_data(ref<String> path,
                     ref<Completer<int>> completer)
            : path(path), completer(completer) {}
        ref<String> path;
        ref<Completer<int>> completer;
    };
};

static void _on_open(uv_fs_t *req)
{
    auto data = reinterpret_cast<File::_File::_open_data *>(req->data);
    if (req->result > -1)
    {
        data->completer->complete(Object::create<File::_File>(data->path, req, data->completer));
        uv_fs_req_cleanup(req);
    }
    else
    {
        data->completer->complete(Object::create<File::Error>(data->path, static_cast<int>(req->result), data->completer));
        uv_fs_req_cleanup(req);
        delete req;
    }
    delete data;
}

ref<Future<ref<File>>> File::fromPath(ref<String> path, OpenFlags flags, OpenMode mode, option<EventLoopGetterMixin> getter)
{
    auto completer = Object::create<Completer<ref<File>>>(getter);
    auto loop = ensureEventLoop(getter);
    auto handle = reinterpret_cast<uv_loop_t *>(loop->nativeHandle());
    uv_fs_t *request = new uv_fs_t;
    request->data = new File::_File::_open_data{path, completer};
    uv_fs_open(handle, request, path->c_str(), flags, mode, _on_open);
    return completer;
}

static void _on_unlink(uv_fs_t *req)
{
    auto data = reinterpret_cast<File::_File::_unlink_data *>(req->data);
    data->completer->complete(static_cast<int>(req->result));
    uv_fs_req_cleanup(req);
    delete data;
    delete req;
}

ref<Future<int>> File::unlink(ref<String> path, option<EventLoopGetterMixin> getter)
{
    auto completer = Object::create<Completer<int>>(getter);
    auto loop = ensureEventLoop(getter);
    auto handle = reinterpret_cast<uv_loop_t *>(loop->nativeHandle());
    uv_fs_t *request = new uv_fs_t;
    request->data = new File::_File::_unlink_data{path, completer};
    uv_fs_unlink(handle, request, path->c_str(), _on_unlink);
    return completer;
}
