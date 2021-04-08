#pragma once

#include <httplib.h>

#include "async.h"
#include "dispatcher.h"

namespace Http
{
    class Client;
    class Server;

    using Request = httplib::Request;
    using Response = httplib::Response;

}; // namespace Http

class Http::Client : public Dispatcher
{
    static ref<ThreadPool> sharedThreadPool();

public:
    class Result : public Object
    {
    public:
        Result() {}
        Result(httplib::Result &&other) {}
    };

    Client(State<StatefulWidget> *state, ref<String> address, unsigned char port = 80)
        : Dispatcher(state), _client(address->c_str(), port) {}

    void dispose() override;

    virtual ref<Future<ref<Result>>> get(ref<String> pattern);
    virtual ref<Future<ref<Result>>> head(ref<String> pattern);
    virtual ref<Future<ref<Result>>> post(ref<String> pattern);
    virtual ref<Future<ref<Result>>> put(ref<String> pattern);
    virtual ref<Future<ref<Result>>> patch(ref<String> pattern);
    virtual ref<Future<ref<Result>>> del(ref<String> pattern);
    virtual ref<Future<ref<Result>>> options(ref<String> pattern);

protected:
    httplib::Client _client;
};

class Http::Server : public Dispatcher
{
public:
    using Handler = httplib::Server::Handler;

    Server(State<StatefulWidget> *state) : Dispatcher(state) {}
    virtual ~Server() { assert(!_server.is_running() && "Http::Server was dropped before dispose. "); }

    virtual Server *listen(ref<String> address, unsigned char port);
    virtual bool isRunning();
    void dispose() override;

    virtual Server *onGet(ref<String> pattern, Handler handler);
    virtual Server *onPost(ref<String> pattern, Handler handler);
    virtual Server *onPut(ref<String> pattern, Handler handler);
    virtual Server *onPatch(ref<String> pattern, Handler handler);
    virtual Server *onDelete(ref<String> pattern, Handler handler);
    virtual Server *onOptions(ref<String> pattern, Handler handler);

    virtual Server *setMountPoint(ref<String> pattern, ref<String> directory);
    virtual Server *removeMountPoint(ref<String> pattern);

protected:
    httplib::Server _server;
};
