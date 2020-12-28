cmake_minimum_required(VERSION 3.10)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src src)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src/basic basic_src)

add_library(async_runtime ${src} ${basic_src})