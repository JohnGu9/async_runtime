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

bool File::exists(String path)
{
    std::ifstream f(path.c_str());
    return f.good();
}

Object::Ref<Future<bool>> File::exists(State<StatefulWidget> *state, String path)
{
    Object::Ref<Completer<bool>> completer = Object::create<Completer<bool>>(state);
    sharedThreadPool()->post([path, completer] {
        //TODO: add read/write lock
        std::ifstream file(path.c_str());
        completer->complete(file.good());
    });
    return completer->future;
}

int File::remove(String path)
{
    return std::remove(path.c_str());
}

Object::Ref<Future<int>> File::remove(State<StatefulWidget> *state, String path)
{
    Object::Ref<Completer<int>> completer = Object::create<Completer<int>>(state);
    sharedThreadPool()->post([path, completer] {
        //TODO: add read/write lock
        completer->complete(std::remove(path.c_str()));
    });
    return completer->future;
}

long long File::size(String path)
{
    std::ifstream::streampos begin, end;
    std::ifstream file(path.toStdString(), std::ios::binary);
    begin = file.tellg();
    file.seekg(0, std::ios::end);
    end = file.tellg();
    file.close();
    return end - begin;
}

Object::Ref<Future<long long>> File::size(State<StatefulWidget> *state, String path)
{
    Object::Ref<Completer<long long>> completer = Object::create<Completer<long long>>(state);
    sharedThreadPool()->post([path, completer] {
        //TODO: add read/write lock
        std::ifstream::streampos begin, end;
        {
            std::ifstream file(path.toStdString(), std::ios::binary);
            begin = file.tellg();
            file.seekg(0, std::ios::end);
            end = file.tellg();
            file.close();
        }
        completer->complete(end - begin);
    });
    return completer->future;
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

Object::Ref<Future<bool>> File::exists()
{
    Object::Ref<File> self = Object::cast<>(this);
    Object::Ref<Completer<bool>> completer = Object::create<Completer<bool>>(_state.get());
    this->_threadPool->post([self, completer] {
        //TODO: add read/write lock
        std::ifstream file(self->_path.c_str());
        completer->complete(file.good());
    });
    return completer->future;
}

Object::Ref<Future<int>> File::remove()
{
    Object::Ref<File> self = Object::cast<>(this);
    Object::Ref<Completer<int>> completer = Object::create<Completer<int>>(_state.get());
    this->_threadPool->post([self, completer] {
        //TODO: add read/write lock
        completer->complete(std::remove(self->_path.c_str()));
    });
    return completer->future;
}

Object::Ref<Future<long long>> File::size()
{
    Object::Ref<File> self = Object::cast<>(this);
    Object::Ref<Completer<long long>> completer = Object::create<Completer<long long>>(_state.get());
    this->_threadPool->post([self, completer] {
        //TODO: add read/write lock
        std::ifstream::streampos begin, end;
        {
            std::ifstream file(self->_path.toStdString(), std::ios::binary);
            begin = file.tellg();
            file.seekg(0, std::ios::end);
            end = file.tellg();
            file.close();
        }
        completer->complete(end - begin);
    });
    return completer->future;
}

Object::Ref<Future<void>> File::append(String str)
{
    Object::Ref<File> self = Object::cast<>(this);
    Object::Ref<Completer<void>> completer = Object::create<Completer<void>>(_state.get());
    this->_threadPool->post([self, completer, str] {
        {
            //TODO: add read/write lock
            std::ofstream file(self->_path.toStdString(), std::ios::app);
            file << str;
            file.close();
        }
        completer->complete();
    });
    return completer->future;
}

Object::Ref<Future<void>> File::overwrite(String str)
{
    Object::Ref<File> self = Object::cast<>(this);
    Object::Ref<Completer<void>> completer = Object::create<Completer<void>>(_state.get());
    this->_threadPool->post([self, completer, str] {
        {
            //TODO: add read/write lock
            std::ofstream file(self->_path.toStdString(), std::ofstream::trunc);
            file << str;
            file.close();
        }
        completer->complete();
    });
    return completer->future;
}

Object::Ref<Future<void>> File::clear()
{
    Object::Ref<File> self = Object::cast<>(this);
    Object::Ref<Completer<void>> completer = Object::create<Completer<void>>(_state.get());
    this->_threadPool->post([self, completer] {
        {
            //TODO: add read/write lock
            std::ofstream file(self->_path.toStdString(), std::ofstream::trunc);
            file.close();
        }
        completer->complete();
    });
    return completer->future;
}

Object::Ref<Future<String>> File::read()
{
    Object::Ref<File> self = Object::cast<>(this);
    Object::Ref<Completer<String>> completer = Object::create<Completer<String>>(_state.get());
    this->_threadPool->post([self, completer] {
        std::string str;
        {
            //TODO: add read/write lock
            std::ifstream file(self->_path.toStdString(), std::ios::in | std::ios::ate);
            std::ifstream::streampos filesize = file.tellg();
            str.reserve(filesize);
            file.seekg(0);
            while (!file.eof())
                str += file.get();
            file.close();
        }
        completer->complete(std::move(str));
    });
    return completer->future;
}

Object::Ref<Stream<String>> File::readAsStream(size_t chip)
{
    Object::Ref<File> self = Object::cast<>(this);
    Object::Ref<Stream<String>> stream = Object::create<Stream<String>>(_state.get());
    this->_threadPool->post([self, stream, chip] {
        {
            //TODO: add read/write lock
            std::ifstream file(self->_path.toStdString(), std::ios::in | std::ios::ate);
            file.seekg(0);
            size_t i;
            while (!file.eof() && self->_isDisposed == false)
            {
                std::string str;
                str.reserve(chip);
                for (i = 0; i < chip && !file.eof(); i++)
                    str += file.get();
                stream->sink(std::move(str));
            }
            file.close();
        }
        stream->close();
    });
    return stream;
}

Object::Ref<Stream<String>> File::readWordAsStream()
{
    Object::Ref<File> self = Object::cast<>(this);
    Object::Ref<Stream<String>> stream = Object::create<Stream<String>>(_state.get());
    this->_threadPool->post([self, stream] {
        String str;
        {
            //TODO: add read/write lock
            std::ifstream file(self->_path.toStdString());
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
            std::ifstream file(self->_path.toStdString());
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