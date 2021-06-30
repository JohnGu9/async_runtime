Async Runtime
===========

A framework write in C++11 (compatible with up to C++17 standard) that similar with Flutter but without build GUI. Just for backend software. Provide object management, software architecture management and async runtime management. 

## 0.2.6-nullsafety

Welcome to nullsafety world! Now framework provide nullsafety feature. [ref](include/async_runtime/basic/ref.h) for non-null object and [option](include/async_runtime/basic/ref.h) for nullable object. 

1) ref, option and weakref for object management (implement base on std::shared_ptr, cross-platform, less bug and nullsafety)

2) widget and context for software architecture management. (Widget tree layout  and lifecycle just similar with Flutter. If you familiar with Flutter, there is nothing new for you to learn)

3) all async api directly build upon context, init async tasks right after State::initState and dispose async resource when State::dispose (so that async work managed by framework). Ease code and less memory lack problem and less state-unknown conflict problem.

4) compatible with Windows/Linux/macOS and more. compatible with C++17/14/11. CMake build rule. 

## Reference

- test case and demo 
<br/>https://github.com/JohnGu9/async_runtime_test

- system request
<br/>[thread](https://en.cppreference.com/w/cpp/thread/thread) support and [openssl](https://www.openssl.org/) support