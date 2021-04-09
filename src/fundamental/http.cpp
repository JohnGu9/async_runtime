#include "async_runtime/fundamental/http.h"

ref<ThreadPool> Http::Client::sharedThreadPool()
{
    static finalref<ThreadPool> singleton = AutoReleaseThreadPool::factory(1, "HttpClientSharedThreadPool");
    return singleton;
}

void Http::Client::dispose()
{
    _client.stop();
}

struct _Result
{
    std::unique_ptr<httplib::Response> res_;
    httplib::Error err_;
    httplib::Headers request_headers_;
};

Http::Client::Result::Result(httplib::Result &&other)
    : response(std::move(reinterpret_cast<_Result &>(other).res_)),
      error(std::move(reinterpret_cast<_Result &>(other).err_)),
      requestHeaders(std::move(reinterpret_cast<_Result &>(other).request_headers_)) {}

ref<String> Http::Client::Result::errorString() const
{
    switch (error)
    {
    case httplib::Success:
        return "Success";
    case httplib::Unknown:
        return "Unknown";
    case httplib::Connection:
        return "Connection";
    case httplib::BindIPAddress:
        return "BindIPAddress";
    case httplib::Read:
        return "Read";
    case httplib::Write:
        return "Write";
    case httplib::ExceedRedirectCount:
        return "ExceedRedirectCount";
    case httplib::Canceled:
        return "Canceled";
    case httplib::SSLConnection:
        return "SSLConnection";
    case httplib::SSLLoadingCerts:
        return "SSLLoadingCerts";
    case httplib::SSLServerVerification:
        return "SSLServerVerification";
    case httplib::UnsupportedMultipartBoundaryChars:
        return "UnsupportedMultipartBoundaryChars";
    case httplib::Compression:
        return "Compression";
    default:
        return "Unknown Error Type";
    }
}

using Res = ref<Http::Client::Result>;

ref<Future<Res>> Http::Client::get(ref<String> pattern)
{
    ref<Completer<Res>> completer = Object::create<Completer<Res>>(_callbackHandler);
    sharedThreadPool()->post([=] {
        completer->complete(Object::create<Result>(_client.Get(pattern->c_str())));
    });
    return completer->future;
}

ref<Future<Res>> Http::Client::head(ref<String> pattern)
{
    ref<Completer<Res>> completer = Object::create<Completer<Res>>(_callbackHandler);
    sharedThreadPool()->post([=] {
        completer->complete(Object::create<Result>(_client.Head(pattern->c_str())));
    });
    return completer->future;
}

ref<Future<Res>> Http::Client::post(ref<String> pattern)
{
    ref<Completer<Res>> completer = Object::create<Completer<Res>>(_callbackHandler);
    sharedThreadPool()->post([=] {
        completer->complete(Object::create<Result>(_client.Post(pattern->c_str())));
    });
    return completer->future;
}

ref<Future<Res>> Http::Client::put(ref<String> pattern)
{
    ref<Completer<Res>> completer = Object::create<Completer<Res>>(_callbackHandler);
    sharedThreadPool()->post([=] {
        completer->complete(Object::create<Result>(_client.Put(pattern->c_str())));
    });
    return completer->future;
}

ref<Future<Res>> Http::Client::patch(ref<String> pattern)
{
    ref<Completer<Res>> completer = Object::create<Completer<Res>>(_callbackHandler);
    sharedThreadPool()->post([=] {
        completer->complete(Object::create<Result>(_client.Patch(pattern->c_str())));
    });
    return completer->future;
}

ref<Future<Res>> Http::Client::del(ref<String> pattern)
{
    ref<Completer<Res>> completer = Object::create<Completer<Res>>(_callbackHandler);
    sharedThreadPool()->post([=] {
        completer->complete(Object::create<Result>(_client.Delete(pattern->c_str())));
    });
    return completer->future;
}

ref<Future<Res>> Http::Client::options(ref<String> pattern)
{
    ref<Completer<Res>> completer = Object::create<Completer<Res>>(_callbackHandler);
    sharedThreadPool()->post([=] {
        completer->complete(Object::create<Result>(_client.Options(pattern->c_str())));
    });
    return completer->future;
}

Http::Server *Http::Server::listen(ref<String> address, int port)
{
    _server.bind_to_port(address->c_str(), port);
    _listenThread = Thread(
        [this] {
            _server.listen_after_bind();
        });
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
    _listenThread.join();
}

Http::Server *Http::Server::onGet(ref<String> pattern, Handler handler)
{
    _server.Get(pattern->c_str(), [this, handler](const Request &request, Response &response) {
        this->run([&] { handler(request, response); }).get();
    });
    return this;
}

Http::Server *Http::Server::onPost(ref<String> pattern, Handler handler)
{
    _server.Post(pattern->c_str(), [this, handler](const Request &request, Response &response) {
        this->run([&] { handler(request, response); }).get();
    });
    return this;
}

Http::Server *Http::Server::onPut(ref<String> pattern, Handler handler)
{
    _server.Put(pattern->c_str(), [this, handler](const Request &request, Response &response) {
        this->run([&] { handler(request, response); }).get();
    });
    return this;
}

Http::Server *Http::Server::onPatch(ref<String> pattern, Handler handler)
{
    _server.Patch(pattern->c_str(), [this, handler](const Request &request, Response &response) {
        this->run([&] { handler(request, response); }).get();
    });
    return this;
}

Http::Server *Http::Server::onDelete(ref<String> pattern, Handler handler)
{
    _server.Delete(pattern->c_str(), [this, handler](const Request &request, Response &response) {
        this->run([&] { handler(request, response); }).get();
    });
    return this;
}

Http::Server *Http::Server::onOptions(ref<String> pattern, Handler handler)
{
    _server.Options(pattern->c_str(), [this, handler](const Request &request, Response &response) {
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