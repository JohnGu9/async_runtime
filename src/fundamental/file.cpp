#include <iostream>
#include <fstream>
#include "async_runtime/fundamental/file.h"
#include "async_runtime/fundamental/async.h"
#include "async_runtime/widgets/stateful_widget.h"

Object::Ref<ThreadPool> File::sharedThreadPool()
{
    static Object::Ref<ThreadPool> sharedThreadPool = AutoReleaseThreadPool::factory(1);
    return sharedThreadPool;
}

File::File(State<StatefulWidget> *state, String path, size_t threads)
    : _path(path), _state(Object::cast<>(state)),
      _lock(threads > 1 ? Object::create<Lock>() /* if multithread read/write, need an actually lock */
                        : Object::create<Lock::InvailedLock>() /* if only one thread, don't need actually lock */),
      Dispatcher(state, (threads == 0 ? sharedThreadPool() : nullptr), threads)
{
}

Object::Ref<File> File::fromPath(State<StatefulWidget> *state, String path, size_t threads)
{
    return Object::create<File>(state, path, threads);
}

File::~File()
{
    assert(this->_isDisposed && "Detect memory leak. Call dispose before release File object. ");
}

Object::Ref<Future<bool>> File::exists()
{
    Object::Ref<File> self = Object::cast<>(this);
    Object::Ref<Completer<bool>> completer = Object::create<Completer<bool>>(_state.get());
    this->post([self, completer] { completer->complete(self->existsSync()); });
    return completer->future;
}

Object::Ref<Future<int>> File::remove()
{
    Object::Ref<File> self = Object::cast<>(this);
    Object::Ref<Completer<int>> completer = Object::create<Completer<int>>(_state.get());
    this->post([self, completer] { completer->complete(self->removeSync()); });
    return completer->future;
}

Object::Ref<Future<long long>> File::size()
{
    Object::Ref<File> self = Object::cast<>(this);
    Object::Ref<Completer<long long>> completer = Object::create<Completer<long long>>(_state.get());
    this->post([self, completer] { completer->complete(self->sizeSync()); });
    return completer->future;
}

bool File::existsSync()
{
    Object::Ref<Lock::SharedLock> readLock = this->_lock->sharedLock();
    std::ifstream f(path.c_str());
    return f.good();
}

int File::removeSync()
{
    Object::Ref<Lock::UniqueLock> writeLock = this->_lock->uniqueLock();
    return std::remove(path.c_str());
}

long long File::sizeSync()
{
    using std::ifstream;
    Object::Ref<Lock::SharedLock> readLock = this->_lock->sharedLock();
    ifstream file(path.toStdString(), std::ios::binary);
    const ifstream::streampos begin = file.tellg();
    file.seekg(0, std::ios::end);
    const ifstream::streampos end = file.tellg();
    file.close();
    return end - begin;
}

Object::Ref<Future<void>> File::append(String str)
{
    Object::Ref<File> self = Object::cast<>(this);
    Object::Ref<Completer<void>> completer = Object::create<Completer<void>>(_state.get());
    this->post([self, completer, str] {
        {
            Object::Ref<Lock::UniqueLock> writeLock = self->_lock->uniqueLock();
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
    this->post([self, completer, str] {
        {
            Object::Ref<Lock::UniqueLock> writeLock = self->_lock->uniqueLock();
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
    this->post([self, completer] {
        {
            Object::Ref<Lock::UniqueLock> writeLock = self->_lock->uniqueLock();
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
    this->post([self, completer] {
        std::string str;
        {
            Object::Ref<Lock::SharedLock> readLock = self->_lock->sharedLock();
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

Object::Ref<Stream<String>> File::readAsStream(size_t segmentationLength)
{
    Object::Ref<File> self = Object::cast<>(this);
    Object::Ref<Stream<String>> stream = Object::create<Stream<String>>(_state.get());
    this->post([self, stream, segmentationLength] {
        {
            Object::Ref<Lock::SharedLock> readLock = self->_lock->sharedLock();
            std::ifstream file(self->_path.toStdString(), std::ios::in | std::ios::ate);
            file.seekg(0);
            size_t i;
            while (!file.eof() && self->_isDisposed == false)
            {
                std::string str;
                str.reserve(segmentationLength);
                for (i = 0; i < segmentationLength && !file.eof(); i++)
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
    this->post([self, stream] {
        String str;
        {
            Object::Ref<Lock::SharedLock> readLock = self->_lock->sharedLock();
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
    this->post([self, stream] {
        {
            Object::Ref<Lock::SharedLock> readLock = self->_lock->sharedLock();
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
    Dispatcher::dispose();
}