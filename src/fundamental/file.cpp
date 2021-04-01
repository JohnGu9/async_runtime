#include <iostream>
#include <fstream>
#include "async_runtime/fundamental/file.h"
#include "async_runtime/fundamental/async.h"
#include "async_runtime/widgets/stateful_widget.h"

static finalref<Lock::InvalidLock> invalidLock = Object::create<Lock::InvalidLock>();

ref<ThreadPool> File::sharedThreadPool()
{
    static finalref<ThreadPool> sharedThreadPool = AutoReleaseThreadPool::factory(1, "FileSharedThreadPool");
    return sharedThreadPool;
}

File::File(State<StatefulWidget> *state, String path, size_t threads)
    : _path(path), _state(Object::cast<>(state)),
      _lock(threads > 1 ? Object::create<Lock>() /* if multithread read/write, need an actually lock */
                        : invalidLock /* if only one thread, don't need actually lock */),
      AsyncDispatcher(state, threads == 0 ? sharedThreadPool() : option<ThreadPool>::null(), threads)
{
}

ref<File> File::fromPath(State<StatefulWidget> *state, String path, size_t threads)
{
    return Object::create<File>(state, path, threads);
}

File::~File()
{
    assert(this->_isDisposed && "Detect memory leak. Call dispose before release File object. ");
}

ref<Future<bool>> File::exists()
{
    ref<File> self = self();
    ref<Completer<bool>> completer = Object::create<Completer<bool>>(_state.get());
    this->post([self, completer] { completer->complete(self->existsSync()); });
    return completer->future;
}

ref<Future<int>> File::remove()
{
    ref<File> self = self();
    ref<Completer<int>> completer = Object::create<Completer<int>>(_state.get());
    this->post([self, completer] { completer->complete(self->removeSync()); });
    return completer->future;
}

ref<Future<long long>> File::size()
{
    ref<File> self = self();
    ref<Completer<long long>> completer = Object::create<Completer<long long>>(_state.get());
    this->post([self, completer] { completer->complete(self->sizeSync()); });
    return completer->future;
}

bool File::existsSync()
{
    option<Lock::SharedLock> readLock = this->_lock->sharedLock();
    std::ifstream f(path.c_str());
    return f.good();
}

int File::removeSync()
{
    option<Lock::UniqueLock> writeLock = this->_lock->uniqueLock();
    return std::remove(path.c_str());
}

long long File::sizeSync()
{
    using std::ifstream;
    option<Lock::SharedLock> readLock = this->_lock->sharedLock();
    ifstream file(path.toStdString(), std::ios::binary);
    const ifstream::streampos begin = file.tellg();
    file.seekg(0, std::ios::end);
    const ifstream::streampos end = file.tellg();
    file.close();
    return end - begin;
}

ref<Future<void>> File::append(String str)
{
    ref<File> self = self();
    ref<Completer<void>> completer = Object::create<Completer<void>>(_state.get());
    this->post([self, completer, str] {
        {
            option<Lock::UniqueLock> writeLock = self->_lock->uniqueLock();
            std::ofstream file(self->_path.toStdString(), std::ios::app);
            file << str;
            file.close();
        }
        completer->complete();
    });
    return completer->future;
}

ref<Future<void>> File::overwrite(String str)
{
    ref<File> self = self();
    ref<Completer<void>> completer = Object::create<Completer<void>>(_state.get());
    this->post([self, completer, str] {
        {
            option<Lock::UniqueLock> writeLock = self->_lock->uniqueLock();
            std::ofstream file(self->_path.toStdString(), std::ofstream::trunc);
            file << str;
            file.close();
        }
        completer->complete();
    });
    return completer->future;
}

ref<Future<void>> File::clear()
{
    ref<File> self = self();
    ref<Completer<void>> completer = Object::create<Completer<void>>(_state.get());
    this->post([self, completer] {
        {
            option<Lock::UniqueLock> writeLock = self->_lock->uniqueLock();
            std::ofstream file(self->_path.toStdString(), std::ofstream::trunc);
            file.close();
        }
        completer->complete();
    });
    return completer->future;
}

ref<Future<String>> File::read()
{
    ref<File> self = self();
    ref<Completer<String>> completer = Object::create<Completer<String>>(_state.get());
    this->post([self, completer] {
        std::string str;
        {
            option<Lock::SharedLock> readLock = self->_lock->sharedLock();
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

ref<Stream<String>> File::readAsStream(size_t segmentationLength)
{
    ref<File> self = self();
    ref<Stream<String>> stream = Object::create<Stream<String>>(_state.get());
    this->post([self, stream, segmentationLength] {
        {
            option<Lock::SharedLock> readLock = self->_lock->sharedLock();
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

ref<Stream<String>> File::readWordAsStream()
{
    ref<File> self = self();
    ref<Stream<String>> stream = Object::create<Stream<String>>(_state.get());
    this->post([self, stream] {
        String str;
        {
            option<Lock::SharedLock> readLock = self->_lock->sharedLock();
            std::ifstream file(self->_path.toStdString());
            while (file >> str && self->_isDisposed == false)
                stream->sink(str);
            file.close();
        }
        stream->close();
    });
    return stream;
}

ref<Stream<String>> File::readLineAsStream()
{
    ref<File> self = self();
    ref<Stream<String>> stream = Object::create<Stream<String>>(_state.get());
    this->post([self, stream] {
        {
            option<Lock::SharedLock> readLock = self->_lock->sharedLock();
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