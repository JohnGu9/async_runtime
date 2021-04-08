cmake_minimum_required(VERSION 3.10)

# message(STATUS "Compiler Supported features = ${CMAKE_CXX_COMPILE_FEATURES}")

if(CMAKE_BUILD_TYPE STREQUAL Debug)
    add_definitions(-DDEBUG)
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall")
endif()
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_EXTENSIONS OFF)
message(STATUS "Async Runtime using CMAKE_CXX_STANDARD: ${CMAKE_CXX_STANDARD}")
message(STATUS "Async Runtime using CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")

find_package(Threads REQUIRED)
if(CMAKE_USE_WIN32_THREADS_INIT)
    message(STATUS "Async Runtime using Threads: win32")
elseif(CMAKE_USE_PTHREADS_INIT)
    message(STATUS "Async Runtime using Threads: pthread")
elseif(CMAKE_HP_PTHREADS_INIT)
    message(STATUS "Async Runtime using Threads: HP thread")
endif()
message(STATUS "Async Runtime using Threads libraries: ${CMAKE_THREAD_LIBS_INIT}")

find_package(OpenSSL REQUIRED)
message(STATUS "Async Runtime using OpenSSL version: ${OPENSSL_VERSION}")
message(STATUS "Async Runtime using OpenSSL include: ${OPENSSL_INCLUDE_DIR}")
message(STATUS "Async Runtime using OpenSSL libraries: ${OPENSSL_LIBRARIES}")

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

if(MSVC) # in msvc, async_runtime not support shared library mode
add_library(async_runtime 
    ${src} ${basic_src} ${contexts_src} ${fundamental_src} ${elements_src} ${widgets_src})
else()
add_library(async_runtime SHARED 
    ${src} ${basic_src} ${contexts_src} ${fundamental_src} ${elements_src} ${widgets_src})
endif()
target_link_libraries(async_runtime ${CMAKE_THREAD_LIBS_INIT} ${OPENSSL_LIBRARIES})


