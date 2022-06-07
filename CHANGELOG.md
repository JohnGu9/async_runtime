# AsyncRuntime [0.3.0-nullsafety]

## Break Change

- [Http](include/async_runtime/fundamental/) was removed. It's too complex for new async underlay implement so it can not be functional very soon. So I have to remove it for a stable version software. But it will come back later.
- [File](include/async_runtime/fundamental/file.h) apis changed.
- [Udp](include/async_runtime/io/udp.h) was added.
- [Tcp](include/async_runtime/io/tcp.h) was added.

<br/>

## New Stuff

- [EventLoop](include/async_runtime/fundamental/event_loop.h) new async implement [EventLoop] just like python's asyncio that bases on [libuv](vendor/libuv).
- All async components no longer require [ref<State<...>>]() or [ref< ThreadPool>]() argument because the new async implement. And you can pass a optional argument [EventLoopGetterMixin](include/async_runtime/fundamental/event_loop.h) to force the instance to work on the selected event loop.

<br/>

- [Future::wait](include/async_runtime/fundamental/async/future.h) wait all the futures completed.
- [Future::race](include/async_runtime/fundamental/async/future.h) complete when any futures completed.

<br/>

## Known Bugs or Issues

- [Map<Key, Value>](include/async_runtime/utilities/container/map.h) can't use [ref](include/async_runtime/utilities/ref.h) as [Value](include/async_runtime/utilities/container/map.h) type when you use Map::operator[] to assign new pair. Use [lateref](include/async_runtime/utilities/ref.h) to bypass this bug. For example, ref<Map\<int, ref\<String>>> is not allow, but ref<Map\<int, lateref\<String>>> is ok. Or follow the solutions from [these answers](https://stackoverflow.com/questions/1935139/using-stdmapk-v-where-v-has-no-usable-default-constructor).
- [container](include/async_runtime/utilities/container.h) must call [Object::create](include/async_runtime/object/object.h) to init a empty container rather than "{}". If use "{}" will cause empty reference (For example: "ref<Set\<int>> set = {}; " will cause empty refer error). It's C++ syntax limit. Using "{}" will call default empty constructor ([ref<Set\<int>>::ref()]()) rather than std::initializer_list constructor ([ref<Set\<int>>::ref(std::initializer_list<T>&& list)]()).
