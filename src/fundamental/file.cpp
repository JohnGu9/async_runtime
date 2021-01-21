#include "async_runtime/fundamental/file.h"

std::future<void> File::append(const String &str)
{
    Object::Ref<File> self = Object::cast<>(this);
    return this->_threadPool->post([self, str] {
        std::ofstream file(self->_path);
        if (self->_isDisposed == false)
            file << str;
        file.close();
    });
}

std::future<void> File::append(String &&str)
{
    Object::Ref<File> self = Object::cast<>(this);
    return this->_threadPool->post([self](String &str) {
        std::ofstream file(self->_path);
        if (self->_isDisposed == false)
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
        if (self->_isDisposed == false)
            file << str;
        file.close();
    });
}

std::future<void> File::overwrite(String &&str)
{
    Object::Ref<File> self = Object::cast<>(this);
    return this->_threadPool->post([self](String &str) {
        std::ofstream file(self->_path, std::ofstream::trunc);
        if (self->_isDisposed == false)
            file << str;
        file.close();
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

std::future<void> File::read(Fn<void(String &)> onRead)
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

std::future<void> File::readWordAsStream(Fn<void(String &)> onRead)
{
    Object::Ref<File> self = Object::cast<>(this);
    return this->_threadPool->post([self, onRead] {
        String str;
        std::ifstream file(self->_path);
        while (file >> str && self->_isDisposed == false)
            self->_callbackHandler->post(onRead, str);
    });
}

std::future<void> File::readLineAsStream(Fn<void(String &)> onRead)
{
    Object::Ref<File> self = Object::cast<>(this);
    return this->_threadPool->post([self, onRead] {
        String str;
        std::ifstream file(self->_path);
        while (getline(file, str) && self->_isDisposed == false)
            self->_callbackHandler->post(onRead, str);
    });
}

void File::dispose()
{
    this->_isDisposed = true;
    Dispatcher::dispose();
}