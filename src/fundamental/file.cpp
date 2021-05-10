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

File::File(State<StatefulWidget> *state, ref<String> path, size_t threads)
    : AsyncDispatcher(state, threads == 0 ? sharedThreadPool() : option<ThreadPool>::null(), threads),
      _path(path), _state(Object::cast<>(state)),
      _lock(threads > 1 ? Object::create<Lock>() /* if multithread read/write, need an actually lock */
                        : invalidLock /* if only one thread, don't need actually lock */)

{
}

ref<File> File::fromPath(State<StatefulWidget> *state, ref<String> path, size_t threads)
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
    this->post([=] { completer->complete(existsSync()); });
    return completer->future;
}

ref<Future<int>> File::remove()
{
    ref<File> self = self();
    ref<Completer<int>> completer = Object::create<Completer<int>>(_state.get());
    this->post([=] { completer->complete(removeSync()); });
    return completer->future;
}

ref<Future<long long>> File::size()
{
    ref<File> self = self();
    ref<Completer<long long>> completer = Object::create<Completer<long long>>(_state.get());
    this->post([=] { completer->complete(sizeSync()); });
    return completer->future;
}

bool File::existsSync()
{
    option<Lock::SharedLock> readLock = this->_lock->sharedLock();
    std::ifstream f(path->c_str());
    return f.good();
}

int File::removeSync()
{
    option<Lock::UniqueLock> writeLock = this->_lock->uniqueLock();
    return std::remove(path->c_str());
}

long long File::sizeSync()
{
    using std::ifstream;
    option<Lock::SharedLock> readLock = this->_lock->sharedLock();
    ifstream file(path->toStdString(), std::ios::binary);
    const auto begin = file.tellg();
    file.seekg(0, std::ios::end);
    const auto end = file.tellg();
    file.close();
    return end - begin;
}

ref<Future<void>> File::append(ref<String> str)
{
    ref<File> self = self();
    ref<Completer<void>> completer = Object::create<Completer<void>>(_state.get());
    this->post([=] {
        {
            option<Lock::UniqueLock> writeLock = _lock->uniqueLock();
            std::ofstream file(_path->toStdString(), std::ios::app);
            file << str;
            file.close();
        }
        completer->complete();
    });
    return completer->future;
}

ref<Future<void>> File::overwrite(ref<String> str)
{
    ref<File> self = self();
    ref<Completer<void>> completer = Object::create<Completer<void>>(_state.get());
    this->post([=] {
        {
            option<Lock::UniqueLock> writeLock = _lock->uniqueLock();
            std::ofstream file(_path->toStdString(), std::ofstream::trunc);
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
    this->post([=] {
        {
            option<Lock::UniqueLock> writeLock = _lock->uniqueLock();
            std::ofstream file(_path->toStdString(), std::ofstream::trunc);
            file.close();
        }
        completer->complete();
    });
    return completer->future;
}

ref<Future<ref<String>>> File::read()
{
    ref<File> self = self();
    ref<Completer<ref<String>>> completer = Object::create<Completer<ref<String>>>(_state.get());
    this->post([=] {
        std::string str;
        {
            option<Lock::SharedLock> readLock = _lock->sharedLock();
            std::ifstream file(_path->toStdString(), std::ios::in | std::ios::ate);
            file.seekg(0, std::ios::end);
            str.reserve(file.tellg()); // reserve file size
            file.seekg(0, std::ios::beg);
            str.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
            file.close();
        }
        completer->complete(std::move(str));
    });
    return completer->future;
}

ref<Stream<ref<String>>> File::readAsStream(size_t segmentationLength)
{
    ref<File> self = self();
    ref<AsyncStreamController<ref<String>>> controller = Object::create<AsyncStreamController<ref<String>>>(_state.get());
    this->post([=] {
        {
            option<Lock::SharedLock> readLock = _lock->sharedLock();
            std::ifstream file(_path->toStdString(), std::ios::in | std::ios::ate);
            file.seekg(0);
            size_t i;
            while (!file.eof() && _isDisposed == false)
            {
                std::string str;
                str.reserve(segmentationLength);
                for (i = 0; i < segmentationLength && !file.eof(); i++)
                    str += file.get();
                controller->sink(std::move(str));
            }
            file.close();
        }
        controller->close();
    });
    return controller->stream;
}

ref<Stream<ref<String>>> File::readWordAsStream()
{
    ref<File> self = self();
    ref<AsyncStreamController<ref<String>>> controller = Object::create<AsyncStreamController<ref<String>>>(_state.get());
    this->post([=] {
        std::string str;
        {
            option<Lock::SharedLock> readLock = _lock->sharedLock();
            std::ifstream file(_path->toStdString());
            while (file >> str && _isDisposed == false)
                controller->sink(std::move(str));
            file.close();
        }
        controller->close();
    });
    return controller->stream;
}

ref<Stream<ref<String>>> File::readLineAsStream()
{
    ref<File> self = self();
    ref<AsyncStreamController<ref<String>>> controller = Object::create<AsyncStreamController<ref<String>>>(_state.get());
    this->post([=] {
        {
            option<Lock::SharedLock> readLock = _lock->sharedLock();
            std::ifstream file(_path->toStdString());
            lateref<String> str;
            while ((str = getline(file))->isNotEmpty() && _isDisposed == false)
                controller->sink(str);
            file.close();
        }
        controller->close();
    });
    return controller->stream;
}

void File::dispose()
{
    this->_isDisposed = true;
    Dispatcher::dispose();
}