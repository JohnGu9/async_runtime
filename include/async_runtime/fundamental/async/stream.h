#pragma once

#include "../async.h"

template <>
class Stream<std::nullptr_t> : public Object
{
    _ASYNC_RUNTIME_FRIEND_ASYNC_FAMILY;

protected:
    static void completeSync(ref<Completer<void>> onClose)
    {
        onClose->completeSync();
    }

    Stream(ref<ThreadPool> callbackHandler) : _callbackHandler(callbackHandler), _onClose(Object::create<Completer<void>>(callbackHandler)) {}

    ref<ThreadPool> _callbackHandler;
    ref<Completer<void>> _onClose;
    bool _isClosed = false;

public:
    virtual ref<Future<void>> asFuture()
    {
        return this->_onClose->future;
    }

    virtual ~Stream()
    {
        if (!this->_isClosed)
            this->_onClose->completeSync();
    }
};

template <typename T>
class BroadcastStream;

template <>
class Stream<void> : public Stream<std::nullptr_t>
{
    _ASYNC_RUNTIME_FRIEND_ASYNC_FAMILY;

public:
    Stream(ref<ThreadPool> callbackHandler)
        : Stream<std::nullptr_t>(callbackHandler) {}

    virtual ref<StreamSubscription<void>> listen(Function<void()> fn)
    {
        ref<Stream<void>> self = self();
        Function<void()> toListen = [this, self, fn]
        {
            this->_callbackHandler->post([this, self, fn]
                                         {
                                             assert(this->_listener == nullptr && "Single listener stream can't have more than one listener");
                                             this->_listener = fn;
                                             for (size_t c = 0; c < this->_sinkCounter; c++)
                                                 fn();
                                             this->_sinkCounter = 0;
                                             if (this->_isClosed)
                                                 this->_onClose->completeSync();
                                             this->_onClose->future->than([this, self]
                                                                          { this->_listener = nullptr; });
                                         });
        };
        toListen();
        return Object::create<StreamSubscription<void>>(/* resume: */ toListen,
                                                        /* unsubscription: */ [this, self]
                                                        { this->_listener = nullptr; });
    }

    virtual ref<Stream<void>> onClose(Function<void()> fn)
    {
        this->_onClose->future->than(fn);
        return self();
    }

protected:
    size_t _sinkCounter = 0;
    option<Fn<void()>> _listener;
};

template <typename T>
class Stream : public Stream<std::nullptr_t>
{
    _ASYNC_RUNTIME_FRIEND_ASYNC_FAMILY;

public:
    Stream(ref<ThreadPool> callbackHandler)
        : Stream<std::nullptr_t>(callbackHandler),
          _cache(Object::create<List<T>>()) {}

    virtual ref<StreamSubscription<T>> listen(Function<void(const T &)> fn)
    {
        ref<Stream<T>> self = self();
        Function<void()> toListen = [this, self, fn]
        {
            this->_callbackHandler->post([this, self, fn]
                                         {
                                             assert(this->_listener == nullptr && "Single listener stream can't have more than one listener");
                                             this->_listener = fn;
                                             for (auto &cache : this->_cache)
                                                 fn(cache);
                                             this->_cache->clear();
                                             if (this->_isClosed)
                                                 this->_onClose->completeSync();
                                             this->_onClose->future->than([this, self]
                                                                          { this->_listener = nullptr; });
                                         });
        };
        toListen();
        return Object::create<StreamSubscription<T>>(/* resume: */ toListen,
                                                     /* unsubscription: */ [this, self]
                                                     { this->_listener = nullptr; });
    }

    virtual ref<Stream<T>> onClose(Function<void()> fn)
    {
        this->_onClose->future->than(fn);
        return self();
    }

protected:
    ref<List<T>> _cache;
    option<Fn<void(const T &)>> _listener;
};

template <>
class BroadcastStream<void> : public Stream<void>
{
    _ASYNC_RUNTIME_FRIEND_ASYNC_FAMILY;
    template <typename R>
    friend class BroadcastStreamController;

public:
    BroadcastStream(ref<ThreadPool> callbackHandler)
        : Stream<void>(callbackHandler),
          _listeners(Object::create<Set<Function<void()>>>()) {}

    ref<StreamSubscription<void>> listen(Function<void()> fn) override
    {
        ref<BroadcastStream<void>> self = self();
        Function<void()> toListen = [this, self, fn]
        {
            this->_callbackHandler->post([this, self, fn]
                                         {
                                             this->_listeners->emplace(fn);
                                             for (size_t index = 0; index < _sinkCounter; index++)
                                                 for (auto &func : this->_listeners)
                                                     func();
                                             this->_sinkCounter = 0;
                                             if (this->_isClosed)
                                                 Stream<std::nullptr_t>::completeSync(this->_onClose);
                                             // TODO: clear listen when stream closed
                                         });
        };
        toListen();
        return Object::create<StreamSubscription<void>>(/* resume: */ toListen,
                                                        /* unsubscription: */ [this, self, fn]
                                                        { this->_listeners->erase(this->_listeners->find(fn)); });
    }

protected:
    ref<Set<Function<void()>>> _listeners;
};

template <typename T>
class BroadcastStream : public Stream<T>
{
    _ASYNC_RUNTIME_FRIEND_ASYNC_FAMILY;
    template <typename R>
    friend class BroadcastStreamController;

public:
    BroadcastStream(ref<ThreadPool> callbackHandler)
        : Stream<T>(callbackHandler),
          _listeners(Object::create<Set<Function<void(const T &)>>>()) {}

    ref<StreamSubscription<T>> listen(Function<void(const T &)> fn) override
    {
        ref<BroadcastStream<T>> self = self();
        Function<void()> toListen = [this, self, fn]
        {
            this->_callbackHandler->post([this, self, fn]
                                         {
                                             this->_listeners->emplace(fn);
                                             for (auto &cache : this->_cache)
                                                 for (auto &func : this->_listeners)
                                                     func(cache);
                                             this->_cache->clear();
                                             if (this->_isClosed)
                                                 Stream<std::nullptr_t>::completeSync(this->_onClose);
                                             // TODO: clear listen when stream closed
                                         });
        };
        toListen();
        return Object::create<StreamSubscription<T>>(/* resume: */ toListen,
                                                     /* unsubscription: */ [this, self, fn]
                                                     { this->_listeners->erase(this->_listeners->find(fn)); });
    }

protected:
    ref<Set<Function<void(const T &)>>> _listeners;
};
