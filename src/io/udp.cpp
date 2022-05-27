#include "async_runtime/io/udp.h"

extern "C"
{
#include <uv.h>
}
#ifndef _WIN32
#define ULONG size_t
#endif

class Udp::_Udp : public Udp
{
public:
    struct _send_data
    {
        _send_data(ref<Completer<int>> completer,
                   ref<String> message)
            : completer(completer), message(message) {}
        ref<Completer<int>> completer;
        ref<String> message;
    };

    static void _send_cb(uv_udp_send_t *req, int status)
    {
        auto data = reinterpret_cast<_send_data *>(req->data);
        data->completer->complete(status);
        delete data;
        delete req;
    }

    static void _alloc_cb(uv_handle_t *handle,
                          size_t suggested_size,
                          uv_buf_t *buf)
    {
        auto data = reinterpret_cast<_Udp *>(handle->data);
        auto buffer = new std::string;
        buffer->resize(suggested_size, 0); // @TODO: maybe we can allocate less memory
        buf->base = const_cast<char *>(buffer->c_str());
        buf->len = static_cast<ULONG>(suggested_size);
        data->_buffer = buffer;
    }

    static void _recv_cb(uv_udp_t *handle,
                         ssize_t nread,
                         const uv_buf_t *rcvbuf,
                         const struct sockaddr *addr,
                         unsigned flags)
    {
        auto data = reinterpret_cast<_Udp *>(handle->data);
        auto buffer = data->_buffer;
        buffer->resize(nread);
        ref<String> message(std::move(*buffer));
        delete buffer;
        data->_recv->sink(Object::create<RecvMessage>(addr, message));
    }

    using super = Udp;

    bool _isClosed = false;
    uv_udp_t _handle;
    std::string *_buffer; // recv buffer
    ref<StreamController<ref<RecvMessage>>> _recv;

    _Udp(ref<EventLoop> lp) : super(lp), _recv(Object::create<StreamController<ref<RecvMessage>>>(lp))
    {
        uv_loop_t *loop = reinterpret_cast<uv_loop_t *>(_loop->nativeHandle());
        uv_udp_init(loop, &_handle);
        _handle.data = this;
        _recv->close();
    };
    ~_Udp()
    {
        assert(_isClosed);
    }

    int bind(const struct sockaddr *addr, unsigned int flags) override
    {
        assert(!_isClosed);
        return uv_udp_bind(&_handle, addr, flags);
    }

    int connect(const struct sockaddr *addr) override
    {
        assert(!_isClosed);
        return uv_udp_connect(&_handle, addr);
    }

    ref<Future<int>> send(ref<String> message, const struct sockaddr *addr) override
    {
        assert(!_isClosed);
        auto data = new _send_data{Object::create<Completer<int>>(self()), message};
        uv_udp_send_t *req = new uv_udp_send_t;
        req->data = data;
        uv_buf_t buf = uv_buf_init(const_cast<char *>(message->c_str()), static_cast<unsigned int>(message->length()));
        uv_udp_send(req, &_handle, &buf, 1, addr, _send_cb);
        return data->completer;
    }

    ref<Stream<ref<RecvMessage>>> startRecv() override
    {
        assert(!_isClosed);
        if (_recv->isClosed())
        {
            _recv = Object::create<StreamController<ref<RecvMessage>>>(_loop);
            uv_udp_recv_start(&_handle, _alloc_cb, _recv_cb);
        }
        return _recv;
    }

    int stopRecv() override
    {
        if (!_recv->isClosed())
        {
            auto result = uv_udp_recv_stop(&_handle);
            _recv->close();
            return result;
        }
        return 0;
    }

    bool isClosed() override
    {
        return _isClosed;
    }

    option<_Udp> _refLock = nullptr;

    static void _close_udp(uv_handle_t *handle)
    {
        auto data = reinterpret_cast<_Udp *>(handle->data);
        data->_refLock = nullptr;
    }

    void close() override
    {
        if (!_isClosed)
        {
            _isClosed = true;
            _refLock = self();
            stopRecv();
            uv_close(reinterpret_cast<uv_handle_t *>(&_handle), _close_udp);
        }
    }
};

ref<Udp> Udp::from(const struct sockaddr *bind, unsigned int bindFlags, const struct sockaddr *connect, option<EventLoopGetterMixin> getter)
{
    auto udp = Object::create<Udp::_Udp>(EventLoopGetterMixin::ensureEventLoop(getter));
    if (bind != nullptr)
        assert(udp->bind(bind, bindFlags) == 0);
    if (connect != nullptr)
        assert(udp->connect(connect) == 0);
    return udp;
}
