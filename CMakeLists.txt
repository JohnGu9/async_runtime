project(async_runtime)
cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_EXTENSIONS OFF)
if(WIN32)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
endif()
message(STATUS "Async Runtime using CMAKE_CXX_STANDARD: ${CMAKE_CXX_STANDARD}")
message(STATUS "Async Runtime using CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")

# thread setup
find_package(Threads REQUIRED)
if(CMAKE_USE_WIN32_THREADS_INIT)
set(THREAD_LIBRARY_NAME "win32")
elseif(CMAKE_USE_PTHREADS_INIT)
set(THREAD_LIBRARY_NAME "pthread")
elseif(CMAKE_HP_PTHREADS_INIT)
set(THREAD_LIBRARY_NAME "HP thread")
endif()
message(STATUS "Async Runtime using ThreadLibrary: ${THREAD_LIBRARY_NAME}")

# libuv setup
message(STATUS "Libuv: ")
add_subdirectory(vendor/libuv)
message(STATUS "Libuv end")

# build target
if(PROJECT_IS_TOP_LEVEL)
    set(CMAKE_CXX_STANDARD 11)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()

set(INCLUDE_DIR 
    ${CMAKE_CURRENT_SOURCE_DIR}/include 
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/libuv/include
)

include_directories(${INCLUDE_DIR})

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src src)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src/basic basic_src)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src/contexts contexts_src)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src/fundamental fundamental_src)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src/elements elements_src)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src/widgets widgets_src)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src/io io_src)

set(async_runtime_src ${src} ${basic_src} ${fundamental_src} ${contexts_src} ${elements_src} ${widgets_src})

# async_runtime optional library
option(ASYNC_RUNTIME_WITH_IO "build async runtime lib with io components" OFF)
if(ASYNC_RUNTIME_WITH_IO)
    message(STATUS "Async runtime io components ENABLE")
    set(async_runtime_src ${async_runtime_src} ${io_src})
endif()

if(WIN32)
    add_library(async_runtime ${async_runtime_src}) # in Windows, async_runtime not support shared library mode
    add_library(async_runtime_a ${async_runtime_src})
else()
    add_library(async_runtime SHARED ${async_runtime_src})
    add_library(async_runtime_a ${async_runtime_src})
endif()

target_link_libraries(async_runtime ${CMAKE_THREAD_LIBS_INIT} uv_a)
target_compile_options(async_runtime PRIVATE
     $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:-Wall>
     $<$<CXX_COMPILER_ID:MSVC>:/wd4100 /wd4458>)
