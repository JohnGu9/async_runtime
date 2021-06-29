# Async Runtime

A framework write in C++11 that similar with Flutter but without build GUI. Just for backend software. Provide object management, software architecture management and async runtime management. 

## 0.2.6-nullsafety

Welcome to nullsafety world! Now framework provide nullsafety feature. [ref](include/async_runtime/basic/ref.h) for non-null object and [option](include/async_runtime/basic/ref.h) for nullable object. 

1) ref, option and weakref for object management (implement base on std::shared_ptr, cross-platform, less bug and nullsafety)
2) widget and context for software architecture management. (Widget tree layout  and lifecycle just similar with Flutter. If you familiar with Flutter, there is nothing new for you to learn)
3) all async api directly build in context, init async tasks right after State::initState and dispose async resource when State::dispose (so that async work managed by framework. Ease code and less memory lack problem and state-unknown conflict problem)

- test case and demo 
<br/>https://github.com/JohnGu9/async_runtime_test

- system request
<br/>[thread]() support and [openssl]() support