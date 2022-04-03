#version 450

layout(set = 0, binding = 0)  uniform SelectorInfo {
    uint selected_channel;// currently selected channel
} selector_info;

layout(input_attachment_index = 0, set = 0, binding = 1) uniform subpassInput geometry_diffuse;
layout(input_attachment_index = 1, set = 0, binding = 2) uniform subpassInput geometry_pos;
layout(input_attachment_index = 2, set = 0, binding = 3) uniform subpassInput geometry_normal;
layout(input_attachment_index = 3, set = 0, binding = 4) uniform subpassInput geometry_depth;
layout(input_attachment_index = 4, set = 0, binding = 5) uniform subpassInput shading_ao;

layout(location = 0) in vec2 frag_uv;

layout(location = 0) out vec4 present_color;

//alogrithm from https://en.wikipedia.org/wiki/Lambert_azimuthal_equal-area_projection
vec2 azimuthalEAEncode(vec3 n) {
    float p = sqrt(n.z*8+8);
    return vec2(n.xy/p + 0.5);
}
vec3 azimuthalEADecode(vec2 enc) {
    vec2 fenc = enc*4-2;
    float f = dot(fenc,fenc);
    float g = sqrt(1-f/4);
    vec3 n;
    n.xy = fenc*g;
    n.z = 1-f/2;
    return n;
}

void main() {
    switch (selector_info.selected_channel) {
        case 0:
        present_color = subpassLoad(geometry_diffuse);
        break;
        case 1:
        present_color = vec4(subpassLoad(geometry_depth).x);
        break;
        case 2:
        present_color = subpassLoad(geometry_pos);
        break;
        case 3:
        present_color = vec4(azimuthalEADecode(subpassLoad(geometry_normal).xy), 1.0);
        break;
        case 4:
        present_color = vec4(subpassLoad(geometry_diffuse).w * frag_uv, sqrt(1 - subpassLoad(geometry_depth).x), 1.0);
        break;
        case 5:
        present_color = vec4(subpassLoad(shading_ao).x);
        break;
        default:
        present_color = vec4(1.0, 1.0, 1.0, 1.0);
    }
}
