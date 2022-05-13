#pragma once
#include "async.h"
#include "event_loop.h"

#if defined(_WIN32)
#include <winsock2.h>
#else
#include <sys/socket.h>
#endif

class RecvMessage : public virtual Object
{
public:
    const struct sockaddr *address;
    ref<String> content;

    RecvMessage(const struct sockaddr *addr, ref<String> content)
        : address(addr), content(content) {}
};

class Udp : public virtual Object, public EventLoopGetterMixin
{
protected:
    class _Udp;
    ref<EventLoop> _loop;

    Udp(ref<EventLoop> loop) : _loop(loop){};

public:
    static ref<Udp> from(const struct sockaddr *bind = nullptr, unsigned int bindFlags = -1, const struct sockaddr *connect = nullptr, option<EventLoopGetterMixin> getter = nullptr);
    virtual int bind(const struct sockaddr *addr, unsigned int flags) = 0;
    virtual int connect(const struct sockaddr *addr) = 0;

    virtual ref<Future<int>> send(ref<String> message, const struct sockaddr *addr) = 0;

    virtual ref<Stream<ref<RecvMessage>>> startRecv() = 0;
    virtual int stopRecv() = 0;

    virtual bool isClosed() = 0;
    virtual void close() = 0;

    ref<EventLoop> eventLoop() override { return _loop; }
};