#pragma once

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

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
    using super = Dispatcher;

public:
    class Result : public Object
    {
    public:
        Result(httplib::Result &&other);

        const std::unique_ptr<Response> response;
        const httplib::Error error;
        const httplib::Headers requestHeaders;

        ref<String> errorString() const;
    };

    Client(ref<State<StatefulWidget>> state, ref<String> address, int port = 80)
        : Dispatcher(state), _client(new httplib::Client(address->c_str(), port)), httplibClient(*_client) {}

    void dispose() override;

    virtual ref<Future<ref<Result>>> get(ref<String> pattern);
    virtual ref<Future<ref<Result>>> head(ref<String> pattern);
    virtual ref<Future<ref<Result>>> post(ref<String> pattern);
    virtual ref<Future<ref<Result>>> put(ref<String> pattern);
    virtual ref<Future<ref<Result>>> patch(ref<String> pattern);
    virtual ref<Future<ref<Result>>> del(ref<String> pattern);
    virtual ref<Future<ref<Result>>> options(ref<String> pattern);

protected:
    std::unique_ptr<httplib::Client> _client;

public:
    /**
     * @brief 
     * Directly access httplib::Client
     * Be careful callback go outside async runtime thread
     */
    httplib::Client &httplibClient;
};

class Http::Server : public Dispatcher
{
    class TaskQueue;

public:
    using Handler = httplib::Server::Handler;

    Server(ref<State<StatefulWidget>> state);
    virtual ~Server() { assert(!_server.is_running() && "Http::Server was dropped before dispose. "); }

    virtual Server *listen(ref<String> address, int port);
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
    Thread _listenThread;

public:
    /**
     * @brief 
     * Directly access httplib::Server
     * Be careful callback go outside async runtime thread
     */
    httplib::Server &httplibServer = _server;
};
