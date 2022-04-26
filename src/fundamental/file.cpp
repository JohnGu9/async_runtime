#include <iostream>
#include <fstream>
#include "async_runtime/fundamental/async.h"
#include "async_runtime/fundamental/thread_pool.h"
#include "async_runtime/fundamental/file.h"

// @TODO: fix thread not safe bug

static finalref<Lock> invalidLock = Object::create<Lock::InvalidLock>();

static ref<ThreadPool> sharedThreadPool()
{
    static finalref<ThreadPool> sharedThreadPool = AutoReleaseThreadPool::factory(1, "FileSharedThreadPool");
    return sharedThreadPool;
}

File::File(ref<String> path, option<EventLoopGetterMixin> getter)
    : EventLoop::WithHandleMixin(getter), _loop(EventLoopGetterMixin::ensureEventLoop(getter)), _path(path), _lock(invalidLock)
{
}

ref<File> File::fromPath(ref<String> path, option<EventLoopGetterMixin> getter)
{
    return Object::create<File>(path, getter);
}

File::~File()
{
    assert(this->_isDisposed && "Detect memory leak. Call dispose before release File object. ");
}

ref<Future<bool>> File::exists()
{
    ref<File> self = self();
    ref<Completer<bool>> completer = Object::create<Completer<bool>>(self);
    sharedThreadPool()->post([this, self, completer]
                             { completer->resolve(existsSync()); });
    return completer;
}

ref<Future<int>> File::remove()
{
    ref<File> self = self();
    ref<Completer<int>> completer = Object::create<Completer<int>>(self);
    sharedThreadPool()->post([this, self, completer]
                             { completer->resolve(removeSync()); });
    return completer;
}

ref<Future<long long>> File::size()
{
    ref<File> self = self();
    ref<Completer<long long>> completer = Object::create<Completer<long long>>(self);
    sharedThreadPool()->post([this, self, completer]
                             { completer->resolve(sizeSync()); });
    return completer;
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

ref<Future<int>> File::append(ref<String> str)
{
    ref<File> self = self();
    ref<Completer<int>> completer = Object::create<Completer<int>>(self);
    sharedThreadPool()->post([this, self, completer, str]
                             {
        {
            option<Lock::UniqueLock> writeLock = _lock->uniqueLock();
            std::ofstream file(_path->toStdString(), std::ios::app);
            file << str;
            file.close();
        }
        completer->resolve(0); });
    return completer;
}

ref<Future<int>> File::overwrite(ref<String> str)
{
    ref<File> self = self();
    ref<Completer<int>> completer = Object::create<Completer<int>>(self);
    sharedThreadPool()->post([this, self, completer, str]
                             {
        {
            option<Lock::UniqueLock> writeLock = _lock->uniqueLock();
            std::ofstream file(_path->toStdString(), std::ofstream::trunc);
            file << str;
            file.close();
        }
        completer->resolve(0); });
    return completer;
}

ref<Future<int>> File::clear()
{
    ref<File> self = self();
    ref<Completer<int>> completer = Object::create<Completer<int>>(self);
    sharedThreadPool()->post([this, self, completer]
                             {
        {
            option<Lock::UniqueLock> writeLock = _lock->uniqueLock();
            std::ofstream file(_path->toStdString(), std::ofstream::trunc);
            file.close();
        }
        completer->resolve(0); });
    return completer;
}

ref<Future<ref<String>>> File::read()
{
    ref<File> self = self();
    ref<Completer<ref<String>>> completer = Object::create<Completer<ref<String>>>(self);
    sharedThreadPool()->post([this, self, completer]
                             {
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
        completer->resolve(std::move(str)); });
    return completer;
}

ref<Stream<ref<String>>> File::readAsStream(size_t segmentationLength)
{
    ref<File> self = self();
    ref<StreamController<ref<String>>> controller = Object::create<StreamController<ref<String>>>(self);
    sharedThreadPool()->post([this, self, controller, segmentationLength]
                             {
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
                    str += static_cast<char>(file.get());
                controller->sink(std::move(str));
            }
            file.close();
        }
        controller->close(); });
    return controller;
}

ref<Stream<ref<String>>> File::readWordAsStream()
{
    ref<File> self = self();
    ref<StreamController<ref<String>>> controller = Object::create<StreamController<ref<String>>>(self);
    sharedThreadPool()->post([this, self, controller]
                             {
        std::string str;
        {
            option<Lock::SharedLock> readLock = _lock->sharedLock();
            std::ifstream file(_path->toStdString());
            while (file >> str && _isDisposed == false)
                controller->sink(std::move(str));
            file.close();
        }
        controller->close(); });
    return controller;
}

ref<Stream<ref<String>>> File::readLineAsStream()
{
    ref<File> self = self();
    ref<StreamController<ref<String>>> controller = Object::create<StreamController<ref<String>>>(self);
    sharedThreadPool()->post([this, self, controller]
                             {
        {
            option<Lock::SharedLock> readLock = _lock->sharedLock();
            std::ifstream file(_path->toStdString());
            lateref<String> str;
            while ((str = getline(file))->isNotEmpty() && _isDisposed == false)
                controller->sink(str);
            file.close();
        }
        controller->close(); });
    return controller;
}

void File::dispose()
{
    _isDisposed = true;
    handle->dispose();
}
