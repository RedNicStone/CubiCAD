cmake_minimum_required(VERSION 3.15)

file(GLOB_RECURSE STB_LIB_SRC ${CMAKE_SOURCE_DIR}/lib/stb/*.c ${CMAKE_SOURCE_DIR}/lib/stb/*.h)

add_library(STB ${STB_LIB_SRC})
