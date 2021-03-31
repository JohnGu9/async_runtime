cmake_minimum_required(VERSION 3.10)

if(CMAKE_BUILD_TYPE STREQUAL Debug)
    if(MSVC)
        add_compile_definitions(DEBUG)
    else()    
        ADD_DEFINITIONS(-DDEBUG)
    endif()
endif()

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)
find_package(OpenSSL REQUIRED)
include_directories(${OPENSSL_INCLUDE_DIR})
message(STATUS "Using OpenSSL version ${OPENSSL_VERSION}")
message(STATUS "Using OpenSSL include ${OPENSSL_INCLUDE_DIR}")
message(STATUS "Using OpenSSL libraries ${OPENSSL_LIBRARIES}")

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