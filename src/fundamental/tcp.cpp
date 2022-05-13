#include "async_runtime/fundamental/tcp.h"

extern "C"
{
#include <uv.h>
}

class Tcp::_Connection : public Tcp::Connection
{
    using super = Tcp::Connection;

public:
    bool _isClosed = false;
    uv_tcp_t *_handle;
    uv_stream_t *_connection;
    option<Tcp> _tcp;
    ref<StreamController<ref<String>>> _read;

    _Connection(ref<EventLoop> loop, int status, uv_tcp_t *handle, option<Tcp> tcp = nullptr)
        : super(loop, status), _handle(handle), _tcp(tcp),
          _read(Object::create<StreamController<ref<String>>>())
    {
        _connection = reinterpret_cast<uv_stream_t *>(_handle);
        _connection->data = this;

        _read->close();
    }

    virtual ~_Connection()
    {
        assert(_isClosed);
        if (_tcp == nullptr)
            delete _handle;
    }

    bool isClosed() override { return _isClosed; }

    struct _shutdown_data
    {
        _shutdown_data(ref<Tcp::Connection> connection, ref<Completer<int>> completer)
            : connection(connection), completer(completer) {}
        ref<Tcp::Connection> connection;
        ref<Completer<int>> completer;
    };

    static void _on_shutdown(uv_shutdown_t *req, int status)
    {
        auto data = reinterpret_cast<_shutdown_data *>(req->data);
        data->completer->complete(status);
        delete data;
        delete req;
    }

    ref<Future<int>> shutdown() override
    {
        assert(!_isClosed);
        _isClosed = true;

        stopRead();
        auto data = new _shutdown_data{self(), Object::create<Completer<int>>()};
        auto req = new uv_shutdown_t;
        req->data = data;
        uv_shutdown(req, _connection, _on_shutdown);
        return data->completer;
    }

    option<Tcp::Connection> _refLock;
    lateref<Completer<int>> _close;

    static void _on_close(uv_handle_t *handle)
    {
        auto data = reinterpret_cast<_Connection *>(handle->data);
        data->_close->complete(0);
        data->_refLock = nullptr;
    }

    ref<Future<int>> closeReset() override
    {
        assert(!_isClosed);
        _isClosed = true;

        stopRead();
        _close = Object::create<Completer<int>>();
        _refLock = self();
        uv_tcp_close_reset(_handle, _on_close);
        return _close;
    }

    bool isReadable() override { return uv_is_readable(_connection); }

    bool isWritable() override { return uv_is_writable(_connection); }

    std::string *_buffer = nullptr;

    static void _on_alloc(uv_handle_t *handle,
                          size_t suggested_size,
                          uv_buf_t *buf)
    {
        auto data = reinterpret_cast<_Connection *>(handle->data);
        auto buffer = new std::string;
        buffer->resize(suggested_size, 0); // @TODO: maybe we can allocate less memory
        buf->base = const_cast<char *>(buffer->c_str());
        buf->len = static_cast<ULONG>(suggested_size);

#ifndef NDEBUG
        assert(data->_buffer == nullptr);
#endif

        data->_buffer = buffer;
    }

    static void _on_read(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf)
    {
        auto data = reinterpret_cast<_Connection *>(stream->data);
        auto buffer = data->_buffer;

#ifndef NDEBUG
        data->_buffer = nullptr;
#endif

        buffer->resize(nread);
        data->_read->sink(std::move(*buffer));
        delete buffer;
    }

    ref<Stream<ref<String>>> startRead() override
    {
        assert(!_isClosed);
        if (_read->isClosed())
        {
            _read = Object::create<StreamController<ref<String>>>();
            if (uv_read_start(_connection, _on_alloc, _on_read) != 0)
                _read->close();
        }
        return _read;
    }

    int stopRead() override
    {
        if (!_read->isClosed())
        {
            auto data = uv_read_stop(_connection);
            _read->close();
            return data;
        }
        return 0;
    }

    struct _write_data
    {
        _write_data(ref<Completer<int>> completer) : completer(completer) {}
        ref<Completer<int>> completer;
    };

    static void _on_write(uv_write_t *req, int status)
    {
        auto data = reinterpret_cast<_write_data *>(req->data);
        data->completer->complete(status);
        delete data;
        delete req;
    }

    ref<Future<int>> write(ref<String> message) override
    {
        assert(!_isClosed);

        auto data = new _write_data{Object::create<Completer<int>>()};
        auto req = new uv_write_t;
        req->data = data;
        auto buf = uv_buf_init(const_cast<char *>(message->c_str()), static_cast<unsigned int>(message->length()));
        uv_write(req, _connection, &buf, 1, _on_write);
        return data->completer;
    }
};

class Tcp::_Tcp : public Tcp
{
public:
    using super = Tcp;

    bool _isClosed = false;
    uv_tcp_t _handle;

    _Tcp(ref<EventLoop> loop) : super(loop)
    {
        auto lp = reinterpret_cast<uv_loop_t *>(loop->nativeHandle());
        uv_tcp_init(lp, &_handle);
        _handle.data = this;
    }

    virtual ~_Tcp()
    {
        assert(_isClosed);
    }

    int bind(const struct sockaddr *addr, unsigned int flags) override
    {
        assert(!_isClosed);
        return uv_tcp_bind(&_handle, addr, flags);
    }

    struct _connect_data
    {
        _connect_data(ref<_Tcp> tcp) : tcp(tcp) {}
        ref<_Tcp> tcp;
    };

    lateref<Completer<ref<Connection>>> _connect;

    static void _on_connect(uv_connect_t *req, int status)
    {
        auto connectData = reinterpret_cast<_connect_data *>(req->data);
        auto data = connectData->tcp;
        if (status == 0)
            data->_connect->complete(Object::create<Tcp::_Connection>(data->eventLoop(), status, &(data->_handle), data));
        else
        {
            data->_isClosed = false; // transform failed, reactivate tcp
            data->_connect->complete(Object::create<Tcp::Connection::Error>(data->eventLoop(), status));
        }
        delete connectData;
        delete req;
    }

    ref<Future<ref<Connection>>> connect(const struct sockaddr *addr) override
    {
        assert(!_isClosed);
        _isClosed = true; // if connect success, uv_tcp_t's ownship will transform to Tcp::Connection
        // And Tcp's no longer available otherwise connect failed

        _connect = Object::create<Completer<ref<Connection>>>();
        auto connect_req = new uv_connect_t;
        connect_req->data = new _connect_data{self()};
        uv_tcp_connect(connect_req, &_handle, addr, _on_connect);
        return _connect;
    }

    ref<StreamController<ref<Connection>>> _listen = Object::create<StreamController<ref<Connection>>>();

    static void _close_tcp_connection(uv_handle_t *handle)
    {
        delete handle;
    }

    static void _on_listen(uv_stream_t *req, int status)
    {
        auto data = reinterpret_cast<_Tcp *>(req->data);
        auto loop = reinterpret_cast<uv_loop_t *>(data->eventLoop()->nativeHandle());
        auto handle = new uv_tcp_t;
        if (status == 0 &&
            (status = uv_tcp_init(loop, handle)) == 0 &&
            (status = uv_accept(req, reinterpret_cast<uv_stream_t *>(handle))) == 0)
        {
            data->_listen->sink(Object::create<Tcp::_Connection>(data->_listen->eventLoop(), status, handle, nullptr));
        }
        else
        {
            data->_listen->sink(Object::create<Tcp::Connection::Error>(data->_listen->eventLoop(), status));
            if (uv_is_active(reinterpret_cast<uv_handle_t *>(handle)))
                uv_close(reinterpret_cast<uv_handle_t *>(handle), _close_tcp_connection);
        }
    }

    ref<Stream<ref<Connection>>> listen(int backlog) override
    {
        assert(!_isClosed);
        uv_listen(reinterpret_cast<uv_stream_t *>(&_handle), backlog, _on_listen);
        return _listen;
    }

    option<_Tcp> _refLock;

    static void _close_tcp(uv_handle_t *handle)
    {
        auto data = reinterpret_cast<_Tcp *>(handle->data);
        data->_refLock = nullptr;
    }

    void close() override
    {
        if (!_isClosed)
        {
            _isClosed = true;
            _refLock = self();
            _listen->close();
            uv_close(reinterpret_cast<uv_handle_t *>(&_handle), _close_tcp);
        }
    }

    int nodelay(int enable) override
    {
        assert(!_isClosed);
        return uv_tcp_nodelay(&_handle, enable);
    }

    int keepalive(int enable, Duration delay) override
    {
        assert(!_isClosed);
        return uv_tcp_keepalive(&_handle, enable, static_cast<unsigned int>(delay.toSeconds()));
    }
};

ref<Tcp> Tcp::from(const struct sockaddr *bind, unsigned int bindFlags, option<EventLoopGetterMixin> getter)
{
    auto tcp = Object::create<Tcp::_Tcp>(EventLoopGetterMixin::ensureEventLoop(getter));
    if (bind != nullptr)
        assert(tcp->bind(bind, bindFlags) == 0);
    return tcp;
}
