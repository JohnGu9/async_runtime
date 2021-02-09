cmake_minimum_required(VERSION 3.10)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src src)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src/basic basic_src)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src/contexts contexts_src)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src/fundamental fundamental_src)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src/elements elements_src)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src/widgets widgets_src)

add_library(async_runtime SHARED ${src} ${basic_src} ${contexts_src} ${fundamental_src} ${elements_src} ${widgets_src})
target_link_libraries(async_runtime pthread)