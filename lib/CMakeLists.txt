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


SET(RAPIDJSON_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/rapidjson/include")

add_library(rapidjson STATIC)

set_target_properties(rapidjson PROPERTIES LINKER_LANGUAGE CXX)

target_sources(rapidjson PRIVATE
        "${RAPIDJSON_SOURCE_DIR}/rapidjson/allocators.h"
        "${RAPIDJSON_SOURCE_DIR}/rapidjson/cursorstreamwrapper.h"
        "${RAPIDJSON_SOURCE_DIR}/rapidjson/document.h"
        "${RAPIDJSON_SOURCE_DIR}/rapidjson/encodedstream.h"
        "${RAPIDJSON_SOURCE_DIR}/rapidjson/encodings.h"
        "${RAPIDJSON_SOURCE_DIR}/rapidjson/filereadstream.h"
        "${RAPIDJSON_SOURCE_DIR}/rapidjson/filewritestream.h"

        "${RAPIDJSON_SOURCE_DIR}/rapidjson/fwd.h"
        "${RAPIDJSON_SOURCE_DIR}/rapidjson/istreamwrapper.h"
        "${RAPIDJSON_SOURCE_DIR}/rapidjson/memorybuffer.h"

        "${RAPIDJSON_SOURCE_DIR}/rapidjson/memorystream.h"
        "${RAPIDJSON_SOURCE_DIR}/rapidjson/ostreamwrapper.h"
        "${RAPIDJSON_SOURCE_DIR}/rapidjson/pointer.h"
        "${RAPIDJSON_SOURCE_DIR}/rapidjson/prettywriter.h"
        "${RAPIDJSON_SOURCE_DIR}/rapidjson/rapidjson.h"
        "${RAPIDJSON_SOURCE_DIR}/rapidjson/reader.h"
        "${RAPIDJSON_SOURCE_DIR}/rapidjson/schema.h"
        "${RAPIDJSON_SOURCE_DIR}/rapidjson/stream.h"
        "${RAPIDJSON_SOURCE_DIR}/rapidjson/stringbuffer.h"
        "${RAPIDJSON_SOURCE_DIR}/rapidjson/uri.h"
        "${RAPIDJSON_SOURCE_DIR}/rapidjson/writer.h")

target_include_directories(rapidjson
        PUBLIC ${RAPIDJSON_SOURCE_DIR})


SET(ICON_FONT_CPP_HEADERS_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/IconFontCppHeaders")

add_library(IconFontCppHeaders STATIC)

set_target_properties(IconFontCppHeaders PROPERTIES LINKER_LANGUAGE CXX)

target_sources(IconFontCppHeaders PRIVATE
        "${ICON_FONT_CPP_HEADERS_SOURCE_DIR}/IconsFontaudio.h"
        "${ICON_FONT_CPP_HEADERS_SOURCE_DIR}/IconsFontAwesome4.h"
        "${ICON_FONT_CPP_HEADERS_SOURCE_DIR}/IconsFontAwesome5.h"
        "${ICON_FONT_CPP_HEADERS_SOURCE_DIR}/IconsFontAwesome5Brands.h"
        "${ICON_FONT_CPP_HEADERS_SOURCE_DIR}/IconsFontAwesome5Pro.h"
        "${ICON_FONT_CPP_HEADERS_SOURCE_DIR}/IconsFontAwesome5ProBrands.h"
        "${ICON_FONT_CPP_HEADERS_SOURCE_DIR}/IconsForkAwesome.h"
        "${ICON_FONT_CPP_HEADERS_SOURCE_DIR}/IconsKenney.h"
        "${ICON_FONT_CPP_HEADERS_SOURCE_DIR}/IconsMaterialDesign.h")

target_include_directories(IconFontCppHeaders
        PUBLIC ${ICON_FONT_CPP_HEADERS_SOURCE_DIR})