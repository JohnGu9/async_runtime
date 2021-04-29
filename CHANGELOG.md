# AsyncRuntime [0.2.4-nullsafety]

## Break Change
- [ref](include/async_runtime/basic/ref.h) only refer non-null [Object](include/async_runtime/object.h)
- [option](include/async_runtime/basic/ref.h) can refer null [Object](include/async_runtime/object.h)
- [weakref](include/async_runtime/basic/ref.h) new api for weak reference of [Object](include/async_runtime/object.h)

<br/>

- [String](include/async_runtime/basic/string.h) become [Object](include/async_runtime/object.h)
- [Set](include/async_runtime/basic/container/set.h) become [Object](include/async_runtime/object.h)
- [List](include/async_runtime/basic/container/list.h) become [Object](include/async_runtime/object.h)
- [Map](include/async_runtime/basic/container/map.h) become [Object](include/async_runtime/object.h)


<br/>

## New Stuff
- [Http:Client](include/async_runtime/fundamental/http.h) is available. 
- [Http:Server](include/async_runtime/fundamental/http.h) is available. 
- [Http](include/async_runtime/fundamental/http.h) demo please check https://github.com/JohnGu9/async_runtime_test/blob/nullsafety/test/test_fundamental/http.cpp
- [Https]() will be support in the future

<br/>

## Known Bug
- [Map<Key, Value>](include/async_runtime/basic/container/map.h) can't use [ref](include/async_runtime/basic/ref.h) for [Value]() type when you use Map::operator[] to assign new pair. Use [lateref](include/async_runtime/basic/ref.h) to bypass this bug. For example, ref<Map\<int, ref\<String>>> is not allow, but ref<Map\<int, lateref\<String>>> is ok. 
- [container](include/async_runtime/basic/container.h) must call [Object::create](include/async_runtime/object.h) to init a empty container rather than "{}". If use "{}" will cause empty reference (For example: "ref<Set\<int>> set = {}; " will cause empty refer error). It's C++ syntax limit. Using "{}" will call default empty constructor ([ref<Set\<int>>::ref()]()) rather than std::initializer_list constructor ([ref<Set\<int>>::ref(std::initializer_list<T>&& list)]()). 