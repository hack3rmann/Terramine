cmake_minimum_required(VERSION 3.28.2)

add_library(glad STATIC ${CMAKE_CURRENT_SOURCE_DIR}/src/gl.c)

target_include_directories(glad PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_directories(glad PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/src)
