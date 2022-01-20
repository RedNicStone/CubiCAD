cmake_minimum_required(VERSION 3.15)

SET(IMGUI_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/imgui")

add_library(imgui STATIC)

target_sources(imgui PRIVATE
        "${IMGUI_SOURCE_DIR}/imgui.cpp"

        "${IMGUI_SOURCE_DIR}/imgui_demo.cpp"
        "${IMGUI_SOURCE_DIR}/imgui_draw.cpp"
        "${IMGUI_SOURCE_DIR}/imgui_widgets.cpp"
        "${IMGUI_SOURCE_DIR}/imgui_tables.cpp"

        "${IMGUI_SOURCE_DIR}/backends/imgui_impl_vulkan.cpp"
        "${IMGUI_SOURCE_DIR}/backends/imgui_impl_glfw.cpp")

target_include_directories(imgui
        PUBLIC ${IMGUI_SOURCE_DIR}
        PUBLIC ${IMGUI_SOURCE_DIR}/backends)