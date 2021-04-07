#include "async_runtime/fundamental/http.h"

ref<ThreadPool> Http::Client::sharedThreadPool()
{
    static finalref<ThreadPool> singleton = Object::create<ThreadPool>(1, "HttpClientSharedThreadPool");
    return singleton;
}

ref<Future<ref<Http::Client::Result>>> Http::Client::get(ref<String> pattern)
{
    ref<Completer<ref<Result>>> completer = Object::create<Completer<ref<Result>>>(_callbackHandler);
    sharedThreadPool()->post([=] {
        completer->complete(Object::create<Result>(_client.Get(pattern->c_str())));
    });
    return completer->future;
}

ref<Future<ref<Http::Client::Result>>> Http::Client::post(ref<String> pattern)
{
    ref<Completer<ref<Result>>> completer = Object::create<Completer<ref<Result>>>(_callbackHandler);
    sharedThreadPool()->post([=] {
        completer->complete(Object::create<Result>(_client.Post(pattern->c_str())));
    });
    return completer->future;
}

ref<Future<ref<Http::Client::Result>>> Http::Client::put(ref<String> pattern)
{
    ref<Completer<ref<Result>>> completer = Object::create<Completer<ref<Result>>>(_callbackHandler);
    sharedThreadPool()->post([=] {
        completer->complete(Object::create<Result>(_client.Put(pattern->c_str())));
    });
    return completer->future;
}

ref<Future<ref<Http::Client::Result>>> Http::Client::patch(ref<String> pattern)
{
    ref<Completer<ref<Result>>> completer = Object::create<Completer<ref<Result>>>(_callbackHandler);
    sharedThreadPool()->post([=] {
        completer->complete(Object::create<Result>(_client.Patch(pattern->c_str())));
    });
    return completer->future;
}

ref<Future<ref<Http::Client::Result>>> Http::Client::del(ref<String> pattern)
{
    ref<Completer<ref<Result>>> completer = Object::create<Completer<ref<Result>>>(_callbackHandler);
    sharedThreadPool()->post([=] {
        completer->complete(Object::create<Result>(_client.Delete(pattern->c_str())));
    });
    return completer->future;
}

ref<Future<ref<Http::Client::Result>>> Http::Client::options(ref<String> pattern)
{
    ref<Completer<ref<Result>>> completer = Object::create<Completer<ref<Result>>>(_callbackHandler);
    sharedThreadPool()->post([=] {
        completer->complete(Object::create<Result>(_client.Options(pattern->c_str())));
    });
    return completer->future;
}

Http::Server *Http::Server::listen(ref<String> address, unsigned char port)
{
    _server.listen(address->c_str(), port);
    return this;
}

bool Http::Server::isRunning()
{
    return _server.is_running();
}

void Http::Server::dispose()
{
    assert(_server.is_running() && "Http::Server had been closed or never listen before. ");
    _server.stop();
}

Http::Server *Http::Server::onGet(ref<String> pattern, Handler handler)
{
    _server.Get(pattern->c_str(), [&](const Request &request, Response &response) {
        this->run([&] { handler(request, response); }).get();
    });
    return this;
}

Http::Server *Http::Server::onPost(ref<String> pattern, Handler handler)
{
    _server.Post(pattern->c_str(), [&](const Request &request, Response &response) {
        this->run([&] { handler(request, response); }).get();
    });
    return this;
}

Http::Server *Http::Server::onPut(ref<String> pattern, Handler handler)
{
    _server.Put(pattern->c_str(), [&](const Request &request, Response &response) {
        this->run([&] { handler(request, response); }).get();
    });
    return this;
}

Http::Server *Http::Server::onPatch(ref<String> pattern, Handler handler)
{
    _server.Patch(pattern->c_str(), [&](const Request &request, Response &response) {
        this->run([&] { handler(request, response); }).get();
    });
    return this;
}

Http::Server *Http::Server::onDelete(ref<String> pattern, Handler handler)
{
    _server.Delete(pattern->c_str(), [&](const Request &request, Response &response) {
        this->run([&] { handler(request, response); }).get();
    });
    return this;
}

Http::Server *Http::Server::onOptions(ref<String> pattern, Handler handler)
{
    _server.Options(pattern->c_str(), [&](const Request &request, Response &response) {
        this->run([&] { handler(request, response); }).get();
    });
    return this;
}

Http::Server *Http::Server::setMountPoint(ref<String> pattern, ref<String> directory)
{
    _server.set_mount_point(pattern->c_str(), directory->c_str());
    return this;
}

Http::Server *Http::Server::removeMountPoint(ref<String> pattern)
{
    _server.remove_mount_point(pattern->c_str());
    return this;
}