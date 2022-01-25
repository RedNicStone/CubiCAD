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
        "${IMGUI_SOURCE_DIR}/backends/imgui_impl_glfw.cpp"

        "${IMGUI_SOURCE_DIR}/misc/cpp/imgui_stdlib.cpp")

target_include_directories(imgui
        PUBLIC ${IMGUI_SOURCE_DIR}
        PUBLIC ${IMGUI_SOURCE_DIR}/backends
        PUBLIC ${IMGUI_SOURCE_DIR}/misc/cpp)


SET(STB_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/stb")

add_library(stb STATIC)

set_target_properties(stb PROPERTIES LINKER_LANGUAGE C)

target_sources(stb PRIVATE
        "${STB_SOURCE_DIR}/stb_c_lexer.h"
        "${STB_SOURCE_DIR}/stb_connected_components.h"
        "${STB_SOURCE_DIR}/stb_ds.h"
        "${STB_SOURCE_DIR}/stb_dxt.h"
        "${STB_SOURCE_DIR}/stb_easy_font.h"
        "${STB_SOURCE_DIR}/stb_herringbone_wang_tile.h"
        "${STB_SOURCE_DIR}/stb_hexwave.h"

        "${STB_SOURCE_DIR}/stb_image.h"
        "${STB_SOURCE_DIR}/stb_image_resize.h"
        "${STB_SOURCE_DIR}/stb_image_write.h"

        "${STB_SOURCE_DIR}/stb_include.h"
        "${STB_SOURCE_DIR}/stb_leakcheck.h"
        "${STB_SOURCE_DIR}/stb_rect_pack.h"
        "${STB_SOURCE_DIR}/stb_sprintf.h"
        "${STB_SOURCE_DIR}/stb_textedit.h"
        "${STB_SOURCE_DIR}/stb_tilemap_editor.h"
        "${STB_SOURCE_DIR}/stb_truetype.h")

target_include_directories(stb
        PUBLIC ${STB_SOURCE_DIR})