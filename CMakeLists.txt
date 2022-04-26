project(async_runtime)
cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_EXTENSIONS OFF)
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
message(STATUS "Async Runtime using Threads: ${THREAD_LIBRARY_NAME}")

# libuv setup
message(STATUS "Informations from libuv: ")
add_subdirectory(vendor/libuv)

# build target
set(INCLUDE_DIR 
    ${CMAKE_CURRENT_SOURCE_DIR}/include 
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/libuv/include
)

include_directories(${INCLUDE_DIR})

if(PROJECT_IS_TOP_LEVEL)
    set(CMAKE_CXX_STANDARD 11)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src src)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src/basic basic_src)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src/contexts contexts_src)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src/fundamental fundamental_src)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src/elements elements_src)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src/widgets widgets_src)
set(all_src ${src} ${basic_src} ${contexts_src} ${fundamental_src} ${elements_src} ${widgets_src})

if(WIN32)
    add_library(async_runtime ${all_src}) # in Windows, async_runtime not support shared library mode
    add_library(async_runtime_a ${all_src})
else()
    add_library(async_runtime SHARED ${all_src})
    add_library(async_runtime_a ${all_src})
endif()

target_link_libraries(async_runtime ${CMAKE_THREAD_LIBS_INIT} uv_a)
target_compile_options(async_runtime PRIVATE
     $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:-Wall>
     $<$<CXX_COMPILER_ID:MSVC>:/wd4100 /wd4458>)
