cmake_minimum_required(VERSION 3.9 FATAL_ERROR)
project(CubiCAD VERSION 0.2.1)
set(CMAKE_CXX_STANDARD     17
    CXX_STANDARD_REQUIRED  YES)

# configure compile options

# COMPILE_FLAG_USE_UINT32_INDICES -  - accepted value ON or OFF
# ON  - Compile with 32bit integers
# OFF - Compile with 64bit integers
option(COMPILE_FLAG_USE_UINT32_INDICES "Compile using 32 bit integers for indexing voxels. \
                                        Limits the map size and detail. Default is 64 bit" OFF)

# AUTO_LOCATE_VULKAN - accepted value ON or OFF
# ON  - Use CMake to auto locate the Vulkan SDK.
# OFF - Vulkan SDK path can be specified manually. This is helpful to test the build on various Vulkan version.
option(AUTO_LOCATE_VULKAN "Automatically locate vulkan using CMake" ON)

# COMPILE_FLAG_OFFLINE - configure make to build offline
# ON  - Try to build offline
# OFF - Build as usual
option(COMPILE_FLAG_OFFLINE "Configure make to build offline" OFF)

if(COMPILE_FLAG_USE_UINT32_INDICES)
    add_compile_definitions(USE_UINT32_INDICES)
endif()

# set output directories for release and debug
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/../)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/../)

# include directories
#include_directories(${CMAKE_SOURCE_DIR}/lib)

# static analysis (only debug)
include(${CMAKE_SOURCE_DIR}/cmake/StaticAnalysis.cmake)

# executable
file(GLOB_RECURSE GRAPHICS_LIB_SRC ${CMAKE_SOURCE_DIR}/src/renderer/*.cpp)
file(GLOB_RECURSE VOXEL_LIB_SRC ${CMAKE_SOURCE_DIR}/src/voxel/*.cpp)

add_executable(CubiCAD
        ${CMAKE_SOURCE_DIR}/src/tests/vulkan_voxel_rasterizer.cpp
        ${GRAPHICS_LIB_SRC}
        ${VOXEL_LIB_SRC})

#set(SOURCE_FILES ${VULKAN_SRC})
#target_sources(CubiCAD PRIVATE
#        ${SOURCE_FILES}
#        )

# set compiler warnings
include(${CMAKE_SOURCE_DIR}/cmake/CompilerWarnings.cmake)
set_project_warnings(CubiCAD)

# sanitizer options if supported by compiler (only debug)
include(${CMAKE_SOURCE_DIR}/cmake/Sanitizers.cmake)
enable_sanitizers(CubiCAD)

# ImGui Library
#[[
set(IMGUI_PATH  ${CMAKE_SOURCE_DIR}/library/imgui)

file(GLOB IMGUI_SOURCES
        ${IMGUI_PATH}/*.cpp)

file(GLOB IMGUI_MISC_SOURCES
        ${IMGUI_PATH}/misc/*.cpp)

set(IMGUI_SOURCES
        ${IMGUI_SOURCES}
        ${IMGUI_MISC_SOURCES}
        ${IMGUI_PATH}/backends/imgui_impl_vulkan.cpp
        ${IMGUI_PATH}/backends/imgui_impl_glfw.cpp)

add_library(ImGui STATIC ${IMGUI_SOURCES})
target_include_directories(ImGui PUBLIC ${IMGUI_PATH})
# sanitizer options if supported by compiler (only debug)
#enable_sanitizers(ImGui)

# link ImGui
target_link_libraries(CubiCAD PUBLIC ImGui)
]]#

# configure shaderc

SET(SHADERC_SKIP_INSTALL ON CACHE BOOL "Disable shaderc tests" FORCE)
SET(SHADERC_SKIP_TESTS ON CACHE BOOL "Disable shaderc tests" FORCE)
SET(SHADERC_SKIP_EXAMPLES ON CACHE BOOL "Disable shaderc tests" FORCE)

# find vulkan
# script source: https://github.com/PacktPublishing/Learning-Vulkan

if(AUTO_LOCATE_VULKAN)
    message(STATUS "Attempting auto locate Vulkan using CMake......")

    # Find Vulkan Path using CMake's Vulkan Module
    # This will return Boolean 'Vulkan_FOUND' indicating the status of find as success(ON) or fail(OFF).
    # Include directory path - 'Vulkan_INCLUDE_DIRS' and 'Vulkan_LIBRARY' with required libraries.
    find_package(Vulkan)

    # Try extracting VulkanSDK path from ${Vulkan_INCLUDE_DIRS}
    if (NOT ${Vulkan_INCLUDE_DIRS} STREQUAL "")
        set(VULKAN_PATH ${Vulkan_INCLUDE_DIRS})
        STRING(REGEX REPLACE "/Include" "" VULKAN_PATH ${VULKAN_PATH})
    endif()

    if(NOT Vulkan_FOUND)
        # CMake may fail to locate the libraries but could be able to
        # provide some path in Vulkan SDK include directory variable
        # 'Vulkan_INCLUDE_DIRS', try to extract path from this.
        message(STATUS "Failed to locate Vulkan SDK, retrying again...")
        if(EXISTS "${VULKAN_PATH}")
            message(STATUS "Successfully located the Vulkan SDK: ${VULKAN_PATH}")
        else()
            message("Error: Unable to locate Vulkan SDK. Please turn off auto locate option by specifying 'AUTO_LOCATE_VULKAN' as 'OFF'")
            message("and specify manually path using 'VULKAN_SDK' and 'VULKAN_VERSION' variables in the CMakeLists.txt.")
            return()
        endif()
    endif()
else()
    message(STATUS "Attempting to locate Vulkan SDK using manual path......")
    set(VULKAN_SDK "C:/VulkanSDK")
    set(VULKAN_VERSION "1.0.33.0")
    set(VULKAN_PATH "${VULKAN_SDK}/${VULKAN_VERSION}")
    message(STATUS "Using manual specified path: ${VULKAN_PATH}")

    # Check if manual set path exists
    if(NOT EXISTS "${VULKAN_PATH}")
        message("Error: Unable to locate this Vulkan SDK path VULKAN_PATH: ${VULKAN_PATH}, please specify correct path.
		For more information on correct installation process, please refer to subsection 'Getting started with Lunar-G SDK'
		and 'Setting up first project with CMake' in Chapter 3, 'Shaking hands with the device' in this book 'Learning Vulkan', ISBN - 9781786469809.")
        return()
    endif()
endif()

find_package(Vulkan REQUIRED FATAL_ERROR)


# find glfw
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(${CMAKE_SOURCE_DIR}/lib/glfw)

# add vma
add_subdirectory(${CMAKE_SOURCE_DIR}/lib/VulkanMemoryAllocator)

# add EventBus
add_subdirectory(${CMAKE_SOURCE_DIR}/lib/EventBus/lib)

# add libmorton
add_subdirectory(${CMAKE_SOURCE_DIR}/lib/libmorton)

# add documentation targets
add_subdirectory(${CMAKE_SOURCE_DIR}/docs)


# build shaderc
set(SHADERC_SOURCE ${CMAKE_SOURCE_DIR}/lib/shaderc)

message(STATUS "Downloading shaderc dependencies")
execute_process(COMMAND ${SHADERC_SOURCE}/utils/git-sync-deps)

add_subdirectory(${SHADERC_SOURCE})

set(GLSLC_EXECUTABLE ${CMAKE_SOURCE_DIR}/glslc)

# build shaders
set(SHADERS_SOURCE_DIR ${CMAKE_SOURCE_DIR}/resources/shaders)
set(SHADERS_BUILD_DIR ${SHADERS_SOURCE_DIR}/compiled)

file(GLOB
    SHADERS_SOURCE_PATH
    LIST_DIRECTORIES false
    RELATIVE ${SHADERS_SOURCE_DIR}
    "${SHADERS_SOURCE_DIR}/*"
)

message(STATUS "Detected shaders to build: ${SHADERS_SOURCE_PATH}")

foreach(SHADER_SOURCE_FILE ${SHADERS_SOURCE_PATH})
    set(SHADER_BINARY_FILE ${SHADER_SOURCE_FILE})
    string(APPEND SHADER_BINARY_FILE ".spv")
    list(APPEND SHADERS_BUILD_PATH "${SHADERS_BUILD_DIR}/${SHADER_BINARY_FILE}")
    add_custom_command(OUTPUT ${SHADERS_BUILD_DIR}/${SHADER_BINARY_FILE}
        COMMAND echo "compiling shader: ${SHADER_SOURCE_FILE}" &
            ${GLSLC_EXECUTABLE} ${SHADERS_SOURCE_DIR}/${SHADER_SOURCE_FILE}
            -o ${SHADERS_BUILD_DIR}/${SHADER_BINARY_FILE}
        DEPENDS ${SHADERS_SOURCE_DIR}/${SHADER_SOURCE_FILE}
    )
endforeach()

add_custom_target(shaders
    ALL
    DEPENDS ${SHADERS_BUILD_PATH}
)

add_dependencies(shaders glslc_exe)
add_dependencies(CubiCAD shaders)

# linking
target_include_directories(
        CubiCAD
        PUBLIC glfw
        PUBLIC Vulkan::Vulkan
        PUBLIC VulkanMemoryAllocator
        PUBLIC Dexode::EventBus
        PUBLIC libmorton
)

target_link_directories(
        CubiCAD
        PUBLIC glfw
        PUBLIC Vulkan::Vulkan
        PUBLIC VulkanMemoryAllocator
        PUBLIC Dexode::EventBus
        PUBLIC libmorton
)

target_link_libraries(
        CubiCAD

        glfw
        Vulkan::Vulkan
        VulkanMemoryAllocator
        Dexode::EventBus
        libmorton
)

# make
if(COMPILE_FLAG_OFFLINE)
    set(CMAKE_MAKE_PROGRAM ${CMAKE_MAKE_PROGRAM} -Dbuiltin_xrootd=OFF)
endif()

# CPack
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)