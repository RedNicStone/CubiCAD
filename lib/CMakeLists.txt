cmake_minimum_required(VERSION 3.15)
project(lib)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_library(lib STATIC
        VulkanMemoryAllocator/include/vk_mem_alloc.h
        )
