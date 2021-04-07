cmake_minimum_required(VERSION 3.10)

# message(STATUS "Compiler Supported features = ${CMAKE_CXX_COMPILE_FEATURES}")

if(CMAKE_BUILD_TYPE STREQUAL Debug)
    add_definitions(-DDEBUG)
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall")
endif()
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_EXTENSIONS OFF)
message(STATUS "Using CMAKE_CXX_STANDARD: ${CMAKE_CXX_STANDARD}")
message(STATUS "Using CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")

find_package(Threads REQUIRED)
if(CMAKE_USE_WIN32_THREADS_INIT)
    message(STATUS "Using Threads: win32")
elseif(CMAKE_USE_PTHREADS_INIT)
    message(STATUS "Using Threads: pthread")
elseif(CMAKE_HP_PTHREADS_INIT)
    message(STATUS "Using Threads: HP thread")
endif()
message(STATUS "Using Threads libraries: ${CMAKE_THREAD_LIBS_INIT}")

find_package(OpenSSL REQUIRED)
message(STATUS "Using OpenSSL version: ${OPENSSL_VERSION}")
message(STATUS "Using OpenSSL include: ${OPENSSL_INCLUDE_DIR}")
message(STATUS "Using OpenSSL libraries: ${OPENSSL_LIBRARIES}")

include_directories(${OPENSSL_INCLUDE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/vendor/cpp-httplib)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src src)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src/basic basic_src)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src/contexts contexts_src)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src/fundamental fundamental_src)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src/elements elements_src)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src/widgets widgets_src)

if(MSVC) # in msvc, async_runtime not support shared library mode
add_library(async_runtime 
    ${src} ${basic_src} ${contexts_src} ${fundamental_src} ${elements_src} ${widgets_src})
else()
add_library(async_runtime SHARED 
    ${src} ${basic_src} ${contexts_src} ${fundamental_src} ${elements_src} ${widgets_src})
endif()
target_link_libraries(async_runtime ${CMAKE_THREAD_LIBS_INIT} ${OPENSSL_LIBRARIES})

message(STATUS "Using Async Runtime include: ${CMAKE_CURRENT_SOURCE_DIR}/include")
message(STATUS "Using Async Runtime libraries: async_runtime")