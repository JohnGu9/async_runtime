project(async_runtime)
cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_EXTENSIONS OFF)

if(WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
endif()

message(STATUS "Async Runtime using CMAKE_CXX_STANDARD: ${CMAKE_CXX_STANDARD}")
message(STATUS "Async Runtime using CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")

# thread setup
option(ASYNC_RUNTIME_WITH_THREAD_LIBRARY "build async runtime lib with system thread library" ON) # async_runtime optional library

if(ASYNC_RUNTIME_WITH_THREAD_LIBRARY)
    find_package(Threads REQUIRED)

    if(CMAKE_USE_WIN32_THREADS_INIT)
        set(THREAD_LIBRARY_NAME "win32")
    elseif(CMAKE_USE_PTHREADS_INIT)
        set(THREAD_LIBRARY_NAME "pthread")
    elseif(CMAKE_HP_PTHREADS_INIT)
        set(THREAD_LIBRARY_NAME "HP thread")
    endif()

    message(STATUS "Async Runtime using ThreadLibrary: ${THREAD_LIBRARY_NAME}")
endif()

if(WIN32)
# windows only build static library, no need PIC
else()
    set(CMAKE_POSITION_INDEPENDENT_CODE ON) # thread_local require PIC
endif()

# libuv setup
message(STATUS "Libuv: ")
add_subdirectory(vendor/libuv)

if(WIN32)
    # disable libuv useless warnings on windows platform
    target_compile_options(uv PRIVATE /wd4244 /wd4245 /wd4267 /wd4701 /wd4702)
    target_compile_options(uv_a PRIVATE /wd4244 /wd4245 /wd4267 /wd4701 /wd4702)
endif()

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

# object
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src/object object_src)
add_library(async_runtime_object ${object_src})

# utilities
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src/utilities utilities_src)
add_library(async_runtime_utilities ${utilities_src})
target_link_libraries(async_runtime_utilities async_runtime_object)

# fundamental
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src/fundamental fundamental_src)
option(ASYNC_RUNTIME_WITH_IO "build async runtime lib with io components" OFF) # async_runtime optional library

if(ASYNC_RUNTIME_WITH_IO)
    aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src/io io_src)
    message(STATUS "Async runtime io components ENABLE")
    set(fundamental_src ${fundamental_src} ${io_src})
endif()

add_library(async_runtime_fundamental ${fundamental_src})
target_link_libraries(async_runtime_fundamental async_runtime_utilities uv_a)

if(ASYNC_RUNTIME_WITH_THREAD_LIBRARY)
    target_link_libraries(async_runtime_fundamental ${CMAKE_THREAD_LIBS_INIT})
endif()

# async_runtime
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src/elements elements_src)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src/widgets widgets_src)
set(async_runtime_src ${elements_src} ${widgets_src})

if(WIN32)
    add_library(async_runtime ${async_runtime_src}) # in Windows, async_runtime not support shared library mode
    add_library(async_runtime_a ${async_runtime_src})
else()
    add_library(async_runtime SHARED ${async_runtime_src})
    add_library(async_runtime_a ${async_runtime_src})
endif()

target_link_libraries(async_runtime async_runtime_fundamental)
