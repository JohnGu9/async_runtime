# AsyncRuntime [0.2.2-nullsafety]

## Break Change
- [ref](include/async_runtime/basic/ref.h) only refer non-null [Object](include/async_runtime/object.h)
- [option](include/async_runtime/basic/ref.h) can refer null [Object](include/async_runtime/object.h)
- [weakref](include/async_runtime/basic/ref.h) new api for weak reference of [Object](include/async_runtime/object.h)

<br/>

- [String](include/async_runtime/basic/string.h) become [Object](include/async_runtime/object.h)
- [ref\<String>](include/async_runtime/basic/string.h) and [option\<String>](include/async_runtime/basic/string.h) replace origin [String]()

<br/>

## New Stuff
- [Http:Client](include/async_runtime/fundamental/http.h) is available. 
- [Http:Server](include/async_runtime/fundamental/http.h) is available. 
- [Http](include/async_runtime/fundamental/http.h) demo please check https://github.com/JohnGu9/async_runtime_test/blob/nullsafety/test/test_fundamental/http.cpp

<br/>

## Known Bug
- [Map<Key, Value>](include/async_runtime/basic/container/map.h) can't use [ref](include/async_runtime/basic/ref.h) for [Value]() type when you use Map::operator[] to assign new pair. Use [lateref](include/async_runtime/basic/ref.h) to bypass this bug. 