#include "async_runtime/fundamental/file.h"
#include "async_runtime/fundamental/async.h"
#include "async_runtime/widgets/stateful_widget.h"

File::File(State<StatefulWidget> *state, String path) : _path(path), _state(Object::cast<State<StatefulWidget>, State<StatefulWidget>>(state))
{
    static Object::Ref<ThreadPool> sharedThreadPool = Object::create<StaticThreadPool>(1);
    this->_threadPool = sharedThreadPool;
}

Object::Ref<File> File::fromPath(State<StatefulWidget> *state, String path)
{
    return Object::create<File>(state, path);
}

File::~File()
{
    assert(this->_isDisposed && "Detect memory leak");
}

Object::Ref<Future<void>> File::append(const String &str)
{
    Object::Ref<File> self = Object::cast<>(this);
    Object::Ref<Completer<void>> completer = Object::create<Completer<void>>(_state.get());
    this->_threadPool->post([self, completer, str] {
        std::ofstream file(self->_path);
        file << std::move(str);
        file.close();
        completer->complete();
    });
    return completer->getFuture();
}

Object::Ref<Future<void>> File::append(String &&str)
{
    Object::Ref<File> self = Object::cast<>(this);
    Object::Ref<Completer<void>> completer = Object::create<Completer<void>>(_state.get());
    this->_threadPool->post([self, completer, str] {
        std::ofstream file(self->_path);
        file << std::move(str);
        file.close();
        completer->complete();
    });
    return completer->getFuture();
}

Object::Ref<Future<void>> File::overwrite(const String &str)
{
    Object::Ref<File> self = Object::cast<>(this);
    Object::Ref<Completer<void>> completer = Object::create<Completer<void>>(_state.get());
    this->_threadPool->post([self, completer, str] {
        std::ofstream file(self->_path, std::ofstream::trunc);
        file << std::move(str);
        file.close();
        completer->complete();
    });
    return completer->getFuture();
}

Object::Ref<Future<void>> File::overwrite(String &&str)
{
    Object::Ref<File> self = Object::cast<>(this);
    Object::Ref<Completer<void>> completer = Object::create<Completer<void>>(_state.get());
    this->_threadPool->post([self, completer, str] {
        std::ofstream file(self->_path, std::ofstream::trunc);
        file << std::move(str);
        file.close();
        completer->complete();
    });
    return completer->getFuture();
}

Object::Ref<Future<void>> File::clear()
{
    Object::Ref<File> self = Object::cast<>(this);
    Object::Ref<Completer<void>> completer = Object::create<Completer<void>>(_state.get());
    this->_threadPool->post([self, completer] {
        std::ofstream file(self->_path, std::ofstream::trunc);
        file.close();
        completer->complete();
    });
    return completer->getFuture();
}

Object::Ref<Future<String>> File::read()
{
    Object::Ref<File> self = Object::cast<>(this);
    Object::Ref<Completer<String>> completer = Object::create<Completer<String>>(_state.get());
    this->_threadPool->post([self, completer] {
        String str;
        std::ifstream file(self->_path, std::ios::in | std::ios::ate);
        std::ifstream::streampos filesize = file.tellg();
        str.reserve(filesize);
        file.seekg(0);
        while (!file.eof())
            str += file.get();

        completer->complete(str);
    });
    return completer->getFuture();
}

Object::Ref<Stream<String>> File::readWordAsStream()
{
    Object::Ref<File> self = Object::cast<>(this);
    Object::Ref<Stream<String>> stream = Object::create<Stream<String>>(_state.get());
    this->_threadPool->post([self, stream] {
        String str;
        std::ifstream file(self->_path);
        while (file >> str && self->_isDisposed == false)
            stream->sink(str);
        stream->close();
    });
    return stream;
}

Object::Ref<Stream<String>> File::readLineAsStream()
{
    Object::Ref<File> self = Object::cast<>(this);
    Object::Ref<Stream<String>> stream = Object::create<Stream<String>>(_state.get());
    this->_threadPool->post([self, stream] {
        String str;
        std::ifstream file(self->_path);
        while (getline(file, str) && self->_isDisposed == false)
            stream->sink(str);
        stream->close();
    });
    return stream;
}

void File::dispose()
{
    this->_isDisposed = true;
}