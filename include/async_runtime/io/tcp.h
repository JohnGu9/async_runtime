#pragma once
#include "../utilities/duration.h"
#include "../fundamental/async.h"

#ifdef _WIN32
#include <winsock2.h>
#else
#include <sys/socket.h>
#endif

class Tcp : public virtual Object, public EventLoopGetterMixin
{
protected:
    class _Tcp;
    class _Connection;
    ref<EventLoop> _loop;
    ref<EventLoop> eventLoop() override { return _loop; }
    Tcp(ref<EventLoop> loop) : _loop(loop) {}

public:
    class Connection : public virtual Object, public EventLoopGetterMixin
    {
    protected:
        ref<EventLoop> _loop;
        int _status;

    public:
        Connection(ref<EventLoop> loop, int status) : _loop(loop), _status(status) {}

        virtual int status() noexcept { return _status; }
        virtual bool isClosed() noexcept { return true; }

        virtual ref<Future<int>> shutdown() { throw NotImplementedError(); }
        virtual ref<Future<int>> closeReset() { throw NotImplementedError(); }

        virtual bool isReadable() { throw NotImplementedError(); }
        virtual bool isWritable() { throw NotImplementedError(); }

        virtual ref<Stream<ref<String>>> startRead() { throw NotImplementedError(); }
        virtual int stopRead() { throw NotImplementedError(); }
        virtual ref<Future<int>> write(ref<String>) { throw NotImplementedError(); }

        ref<EventLoop> eventLoop() override { return _loop; }
        class Error;
    };

    static ref<Tcp> from(const struct sockaddr *bind = nullptr, unsigned int bindFlags = -1, option<EventLoopGetterMixin> getter = nullptr);

    virtual int bind(const struct sockaddr *addr, unsigned int flags) = 0;
    virtual ref<Future<ref<Connection>>> connect(const struct sockaddr *addr) = 0; // client
    virtual ref<Stream<ref<Connection>>> listen(int backlog = 10) = 0;             // server
    virtual void close() = 0;

    virtual int nodelay(int enable) { throw NotImplementedError(); }
    virtual int keepalive(int enable, Duration delay) { throw NotImplementedError(); }
};

class Tcp::Connection::Error : public Tcp::Connection
{
    using super = Tcp::Connection;

public:
    Error(ref<EventLoop> loop, int status) : super(loop, status) {}
};