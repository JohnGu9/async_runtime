cmake_minimum_required(VERSION 3.10)

project(async_runtime)

IF (CMAKE_BUILD_TYPE STREQUAL Debug)
    ADD_DEFINITIONS(-DDEBUG)
ENDIF()

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/async_runtime/include)

add_subdirectory(async_runtime)

# test globally
add_executable(test test/test.cpp)
target_link_libraries(test async_runtime)

# test case
add_executable(test_basic_callback test/test_basic/test_callback.cpp)
target_link_libraries(test_basic_callback async_runtime)

add_executable(test_stateless_widget test/test_widget/stateless_widget.cpp)
target_link_libraries(test_stateless_widget async_runtime)

add_executable(test_multi_child_widget test/test_widget/multi_child_widget.cpp)
target_link_libraries(test_multi_child_widget async_runtime)

add_executable(test_timer test/test_fundamental/timer.cpp)
target_link_libraries(test_timer async_runtime)