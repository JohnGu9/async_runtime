cmake_minimum_required(VERSION 3.10)

# message(STATUS "Compiler Supported features = ${CMAKE_CXX_COMPILE_FEATURES}")
option(ASYNC_RUNTIME_BUILD_SHARED "Build AsyncRuntime Shared Library(only available on unix-like system, not available on win32)" ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_EXTENSIONS OFF)
message(STATUS "Async Runtime using CMAKE_CXX_STANDARD: ${CMAKE_CXX_STANDARD}")
message(STATUS "Async Runtime using CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")

find_package(Threads REQUIRED)
if(CMAKE_USE_WIN32_THREADS_INIT)
set(THREAD_LIBRARY_NAME "win32")
elseif(CMAKE_USE_PTHREADS_INIT)
set(THREAD_LIBRARY_NAME "pthread")
elseif(CMAKE_HP_PTHREADS_INIT)
set(THREAD_LIBRARY_NAME "HP thread")
endif()
message(STATUS "Async Runtime using Threads: ${THREAD_LIBRARY_NAME}")

find_package(OpenSSL)
if(OPENSSL_FOUND)
    message(STATUS "Async Runtime using OpenSSL version: ${OPENSSL_VERSION}")
    message(STATUS "Async Runtime using OpenSSL include: ${OPENSSL_INCLUDE_DIR}")
    message(STATUS "Async Runtime using OpenSSL libraries: ${OPENSSL_LIBRARIES}")
    # add_compile_options(-DCPPHTTPLIB_OPENSSL_SUPPORT)
endif()

set(INCLUDE_DIR 
    ${CMAKE_CURRENT_SOURCE_DIR}/include 
    ${OPENSSL_INCLUDE_DIR} 
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/cpp-httplib
)
# export include directories
set(ASYNC_RUNTIME_INCLUDE_DIR ${INCLUDE_DIR} PARENT_SCOPE)

include_directories(${INCLUDE_DIR})

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src src)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src/basic basic_src)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src/contexts contexts_src)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src/fundamental fundamental_src)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src/elements elements_src)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src/widgets widgets_src)
set(all_src ${src} ${basic_src} ${contexts_src} ${fundamental_src} ${elements_src} ${widgets_src})

if(WIN32) # in Windows, async_runtime not support shared library mode
add_library(async_runtime ${all_src})
else()
    if(ASYNC_RUNTIME_BUILD_SHARED)
        add_library(async_runtime SHARED ${all_src})
    else()
        add_library(async_runtime ${all_src})
    endif()
endif()
target_link_libraries(async_runtime ${CMAKE_THREAD_LIBS_INIT} ${OPENSSL_LIBRARIES})
target_compile_options(async_runtime PRIVATE
     $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:-Wall>
     $<$<CXX_COMPILER_ID:MSVC>:/W4 /wd4100 /wd4458>)
