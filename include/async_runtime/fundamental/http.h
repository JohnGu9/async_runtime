#pragma once

#include "async.h"
#include "dispatcher.h"

namespace Http
{
    class Client;
    class Server;
    class Request;
    class Response;

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

protected:
    Object::Ref<Stream<Http::Response> > _stream;

public:
    const Object::Ref<Stream<Http::Response> > &stream = _stream;
};

class Http::Request : public Object
{
public:
};

class Http::Response : public Object
{
public:
};