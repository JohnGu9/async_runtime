#include <iostream>
#include <fstream>
#include "async_runtime/fundamental/file.h"
#include "async_runtime/fundamental/async.h"
#include "async_runtime/widgets/stateful_widget.h"

Object::Ref<ThreadPool> File::sharedThreadPool()
{
    static Object::Ref<ThreadPool> sharedThreadPool = Object::create<StaticThreadPool>(1);
    return sharedThreadPool;
}

File::File(State<StatefulWidget> *state, String path, size_t threads)
    : _path(path), _state(Object::cast<>(state))
{
    assert(threads < 2 && "C++11 do not feature read-write lock, so File object do not support multithread read-write operation. ");
    if (threads == 0)
        this->_threadPool = sharedThreadPool();
    else
        this->_threadPool = Object::create<StaticThreadPool>(threads);
}

Object::Ref<File> File::fromPath(State<StatefulWidget> *state, String path, size_t threads)
{
    return Object::create<File>(state, path, threads);
}

File::~File()
{
    assert(this->_isDisposed && "Detect memory leak");
}

Object::Ref<Future<void>> File::append(String str)
{
    Object::Ref<File> self = Object::cast<>(this);
    Object::Ref<Completer<void>> completer = Object::create<Completer<void>>(_state.get());
    this->_threadPool->post([self, completer, str] {
        {
            //TODO: add read/write lock
            std::ofstream file(self->_path.getNativeString(), std::ios::app);
            file << str;
            file.close();
        }
        completer->complete();
    });
    return completer->getFuture();
}

Object::Ref<Future<void>> File::overwrite(String str)
{
    Object::Ref<File> self = Object::cast<>(this);
    Object::Ref<Completer<void>> completer = Object::create<Completer<void>>(_state.get());
    this->_threadPool->post([self, completer, str] {
        {
            //TODO: add read/write lock
            std::ofstream file(self->_path.getNativeString(), std::ofstream::trunc);
            file << str;
            file.close();
        }
        completer->complete();
    });
    return completer->getFuture();
}

Object::Ref<Future<void>> File::clear()
{
    Object::Ref<File> self = Object::cast<>(this);
    Object::Ref<Completer<void>> completer = Object::create<Completer<void>>(_state.get());
    this->_threadPool->post([self, completer] {
        {
            //TODO: add read/write lock
            std::ofstream file(self->_path.getNativeString(), std::ofstream::trunc);
            file.close();
        }
        completer->complete();
    });
    return completer->getFuture();
}

Object::Ref<Future<String>> File::read()
{
    Object::Ref<File> self = Object::cast<>(this);
    Object::Ref<Completer<String>> completer = Object::create<Completer<String>>(_state.get());
    this->_threadPool->post([self, completer] {
        std::string str;
        {
            //TODO: add read/write lock
            std::ifstream file(self->_path.getNativeString(), std::ios::in | std::ios::ate);
            std::ifstream::streampos filesize = file.tellg();
            str.reserve(filesize);
            file.seekg(0);
            while (!file.eof())
                str += file.get();
            file.close();
        }
        completer->complete(std::move(str));
    });
    return completer->getFuture();
}

Object::Ref<Stream<String>> File::readWordAsStream()
{
    Object::Ref<File> self = Object::cast<>(this);
    Object::Ref<Stream<String>> stream = Object::create<Stream<String>>(_state.get());
    this->_threadPool->post([self, stream] {
        String str;
        {
            //TODO: add read/write lock
            std::ifstream file(self->_path.getNativeString());
            while (file >> str && self->_isDisposed == false)
                stream->sink(str);
            file.close();
        }
        stream->close();
    });
    return stream;
}

Object::Ref<Stream<String>> File::readLineAsStream()
{
    Object::Ref<File> self = Object::cast<>(this);
    Object::Ref<Stream<String>> stream = Object::create<Stream<String>>(_state.get());
    this->_threadPool->post([self, stream] {
        {
            //TODO: add read/write lock
            std::ifstream file(self->_path.getNativeString());
            while (String str = getline(file) && self->_isDisposed == false)
                stream->sink(str);
            file.close();
        }
        stream->close();
    });
    return stream;
}

void File::dispose()
{
    this->_isDisposed = true;
}