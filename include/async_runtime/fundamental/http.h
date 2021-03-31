#pragma once

#include "async.h"
#include "dispatcher.h"

#define CPPHTTPLIB_OPENSSL_SUPPORT
#include <httplib.h>

namespace Http
{
    class Client;
    class Server;
    using Request = httplib::Request;
    using Response = httplib::Response;

}; // namespace Http

class Http::Client : public Dispatcher
{
public:
    Client(State<StatefulWidget> *state) : Dispatcher(state) {}
};

class Http::Server : public Dispatcher
{
public:
    Server(State<StatefulWidget> *state) : Dispatcher(state) {}
};
