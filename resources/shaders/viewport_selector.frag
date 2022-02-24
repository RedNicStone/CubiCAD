#version 450

layout(set = 0, binding = 0)  uniform SelectorInfo {
    uint selected_channel;// currently selected channel
} selector_info;

layout(input_attachment_index = 0, set = 0, binding = 1) uniform subpassInput geometry_diffuse;
layout(input_attachment_index = 1, set = 0, binding = 2) uniform subpassInput geometry_pos;
layout(input_attachment_index = 2, set = 0, binding = 3) uniform subpassInput geometry_normal;

layout(location = 0) in vec2 frag_uv;

layout(location = 0) out vec4 present_color;

void main() {
    present_color = subpassLoad(geometry_diffuse);
}
