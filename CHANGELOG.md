# AsyncRuntime [0.2.1-nullsafety]

## Break Change
- [ref](include/async_runtime/basic/ref.h) only refer non-null [Object](include/async_runtime/object.h)
- [option](include/async_runtime/basic/ref.h) can refer null [Object](include/async_runtime/object.h)
- [weakref](include/async_runtime/basic/ref.h) new api for weak reference of [Object](include/async_runtime/object.h)

<br/>

- [String](include/async_runtime/basic/string.h) become [Object](include/async_runtime/object.h)
- [ref\<String>](include/async_runtime/basic/string.h) and [option\<String>](include/async_runtime/basic/string.h) replace origin [String]()