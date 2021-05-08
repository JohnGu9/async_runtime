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
    case httplib::Error::Success:
    {
        static finalref<String> success = "Success";
        return success;
    }
    case httplib::Error::Unknown:
    {
        static finalref<String> unknown = "Unknown";
        return unknown;
    }
    case httplib::Error::Connection:
    {
        static finalref<String> connection = "Connection";
        return connection;
    }
    case httplib::Error::BindIPAddress:
    {
        static finalref<String> bindIPAddress = "BindIPAddress";
        return bindIPAddress;
    }
    case httplib::Error::Read:
    {
        static finalref<String> read = "Read";
        return read;
    }
    case httplib::Error::Write:
    {
        static finalref<String> write = "Write";
        return write;
    }
    case httplib::Error::ExceedRedirectCount:
    {
        static finalref<String> exceedRedirectCount = "ExceedRedirectCount";
        return exceedRedirectCount;
    }
    case httplib::Error::Canceled:
    {
        static finalref<String> canceled = "Canceled";
        return canceled;
    }
    case httplib::Error::SSLConnection:
    {
        static finalref<String> SSLConnection = "SSLConnection";
        return SSLConnection;
    }
    case httplib::Error::SSLLoadingCerts:
    {
        static finalref<String> SSLLoadingCerts = "SSLLoadingCerts";
        return SSLLoadingCerts;
    }
    case httplib::Error::SSLServerVerification:
    {
        static finalref<String> SSLServerVerification = "SSLServerVerification";
        return SSLServerVerification;
    }
    case httplib::Error::UnsupportedMultipartBoundaryChars:
    {
        static finalref<String> unsupportedMultipartBoundaryChars = "UnsupportedMultipartBoundaryChars";
        return unsupportedMultipartBoundaryChars;
    }
    case httplib::Error::Compression:
    {
        static finalref<String> compression = "Compression";
        return compression;
    }
    default:
    {
        assert(false && "The enum doesn't exists. ");
        static finalref<String> unknownErrorType = "UnknownErrorType";
        return unknownErrorType;
    }
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

class _ThreadPoolTaskQueue : public httplib::TaskQueue
{
    ref<ThreadPool> _threadPool;

public:
    _ThreadPoolTaskQueue(ref<ThreadPool> threadPool) : _threadPool(threadPool) {}
    void enqueue(std::function<void()> fn) override { _threadPool->post(fn); }
    void shutdown() override { Object::detach(_threadPool); }
};

Http::Server::Server(State<StatefulWidget> *state) : Dispatcher(state)
{
    _server.new_task_queue = [this] { return new _ThreadPoolTaskQueue(this->_callbackHandler); };
}

Http::Server *Http::Server::listen(ref<String> address, int port)
{
    _server.bind_to_port(address->c_str(), port);
    _listenThread = Thread([this] { _server.listen_after_bind(); });
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
    _server.Get(pattern->c_str(), std::move(handler));
    return this;
}

Http::Server *Http::Server::onPost(ref<String> pattern, Handler handler)
{
    _server.Post(pattern->c_str(), std::move(handler));
    return this;
}

Http::Server *Http::Server::onPut(ref<String> pattern, Handler handler)
{
    _server.Put(pattern->c_str(), std::move(handler));
    return this;
}

Http::Server *Http::Server::onPatch(ref<String> pattern, Handler handler)
{
    _server.Patch(pattern->c_str(), std::move(handler));
    return this;
}

Http::Server *Http::Server::onDelete(ref<String> pattern, Handler handler)
{
    _server.Delete(pattern->c_str(), std::move(handler));
    return this;
}

Http::Server *Http::Server::onOptions(ref<String> pattern, Handler handler)
{
    _server.Options(pattern->c_str(), std::move(handler));
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