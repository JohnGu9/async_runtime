#include "async_runtime/fundamental/file.h"

Object::Ref<File> File::fromPath(State<StatefulWidget> *state, String path)
{
    return Object::create<File>(state, std::forward<String>(path));
}

std::future<void> File::append(const String &str)
{
    Object::Ref<File> self = Object::cast<>(this);
    return this->_threadPool->post([self, str] {
        std::ofstream file(self->_path);
        file << str;
        file.close();
    });
}

std::future<void> File::append(String &&str)
{
    Object::Ref<File> self = Object::cast<>(this);
    return this->_threadPool->post([self](String &str) {
        std::ofstream file(self->_path);
        file << str;
        file.close();
    },
                                   str);
}

std::future<void> File::overwrite(const String &str)
{
    Object::Ref<File> self = Object::cast<>(this);
    return this->_threadPool->post([self, str] {
        std::ofstream file(self->_path, std::ofstream::trunc);
        file << str;
        file.close();
    });
}

std::future<void> File::overwrite(String &&str)
{
    Object::Ref<File> self = Object::cast<>(this);
    return this->_threadPool->post([self](String &str) {
        std::ofstream file(self->_path, std::ofstream::trunc);
        file << str;
        file.close();
    },
                                   str);
}

std::future<void> File::append(const String &str, Function<void()> onFinished)
{
    Object::Ref<File> self = Object::cast<>(this);
    return this->_threadPool->post([self, str, onFinished] {
        std::ofstream file(self->_path);
        file << str;
        file.close();
        if (self->_isDisposed == false)
            self->_callbackHandler->post(onFinished);
    });
}

std::future<void> File::append(String &&str, Function<void()> onFinished)
{
    Object::Ref<File> self = Object::cast<>(this);
    return this->_threadPool->post([self, onFinished](String &str) {
        std::ofstream file(self->_path);
        file << str;
        file.close();
        if (self->_isDisposed == false)
            self->_callbackHandler->post(onFinished);
    },
                                   str);
}

std::future<void> File::overwrite(const String &str, Function<void()> onFinished)
{
    Object::Ref<File> self = Object::cast<>(this);
    return this->_threadPool->post([self, str, onFinished] {
        std::ofstream file(self->_path, std::ofstream::trunc);
        file << str;
        file.close();
        if (self->_isDisposed == false)
            self->_callbackHandler->post(onFinished);
    });
}

std::future<void> File::overwrite(String &&str, Function<void()> onFinished)
{
    Object::Ref<File> self = Object::cast<>(this);
    return this->_threadPool->post([self, onFinished](String &str) {
        std::ofstream file(self->_path, std::ofstream::trunc);
        file << str;
        file.close();
        if (self->_isDisposed == false)
            self->_callbackHandler->post(onFinished);
    },
                                   str);
}

std::future<void> File::clear()
{
    Object::Ref<File> self = Object::cast<>(this);
    return this->_threadPool->post([self] {
        std::ofstream file(self->_path, std::ofstream::trunc);
        file.close();
    });
}

std::future<void> File::clear(Function<void()> onFinished)
{
    Object::Ref<File> self = Object::cast<>(this);
    return this->_threadPool->post([self, onFinished] {
        std::ofstream file(self->_path, std::ofstream::trunc);
        file.close();
        if (self->_isDisposed == false)
            self->_callbackHandler->post(onFinished);
    });
}

std::future<void> File::read(Function<void(String &)> onRead)
{
    Object::Ref<File> self = Object::cast<>(this);
    return this->_threadPool->post([self, onRead] {
        String str;
        std::ifstream file(self->_path, std::ios::in | std::ios::ate);
        std::ifstream::streampos filesize = file.tellg();
        str.reserve(filesize);
        file.seekg(0);
        while (!file.eof())
            str += file.get();

        if (self->_isDisposed == false)
            self->_callbackHandler->post(onRead, str);
    });
}

std::future<void> File::readWordAsStream(Function<void(String &)> onRead)
{
    Object::Ref<File> self = Object::cast<>(this);
    return this->_threadPool->post([self, onRead] {
        String str;
        std::ifstream file(self->_path);
        while (file >> str && self->_isDisposed == false)
            self->_callbackHandler->post(onRead, str);
    });
}

std::future<void> File::readLineAsStream(Function<void(String &)> onRead)
{
    Object::Ref<File> self = Object::cast<>(this);
    return this->_threadPool->post([self, onRead] {
        String str;
        std::ifstream file(self->_path);
        while (getline(file, str) && self->_isDisposed == false)
            self->_callbackHandler->post(onRead, str);
    });
}

std::future<void> File::readWordAsStream(Function<void(String &)> onRead, Function<void()> onFinished)
{
    Object::Ref<File> self = Object::cast<>(this);
    return this->_threadPool->post([self, onRead, onFinished] {
        String str;
        std::ifstream file(self->_path);
        while (file >> str && self->_isDisposed == false)
            self->_callbackHandler->post(onRead, str);
        if (self->_isDisposed == false)
            self->_callbackHandler->post(onFinished);
    });
}

std::future<void> File::readLineAsStream(Function<void(String &)> onRead, Function<void()> onFinished)
{
    Object::Ref<File> self = Object::cast<>(this);
    return this->_threadPool->post([self, onRead, onFinished] {
        String str;
        std::ifstream file(self->_path);
        while (getline(file, str) && self->_isDisposed == false)
            self->_callbackHandler->post(onRead, str);
        if (self->_isDisposed == false)
            self->_callbackHandler->post(onFinished);
    });
}

void File::dispose()
{
    this->_isDisposed = true;
    Dispatcher::dispose();
}