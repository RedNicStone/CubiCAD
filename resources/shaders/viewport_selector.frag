#version 450

layout(set = 0, binding = 0)  uniform SelectorInfo {
    uint selected_channel;// currently selected channel
} selector_info;

layout(input_attachment_index = 0, set = 0, binding = 1) uniform subpassInput geometry_diffuse;
layout(input_attachment_index = 1, set = 0, binding = 2) uniform subpassInput geometry_pos;
layout(input_attachment_index = 2, set = 0, binding = 3) uniform subpassInput geometry_normal;
layout(input_attachment_index = 3, set = 0, binding = 4) uniform subpassInput geometry_depth;

layout(location = 0) in vec2 frag_uv;

layout(location = 0) out vec4 present_color;

vec3 calculateScreenSpaceNormal(vec3 p) {
    vec3 dx = dFdx(p);
    vec3 dy = -dFdy(p);
    return normalize(cross(dx, dy));
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
        vec4 normal = subpassLoad(geometry_normal);
        if (normal.z == 0.0) {
            vec3 pos = subpassLoad(geometry_pos).xyz;
            present_color = vec4(calculateScreenSpaceNormal(pos), 1.0);
        } else
        present_color = normal;
        break;
        case 4:
        present_color = vec4(subpassLoad(geometry_diffuse).w * frag_uv, sqrt(1 - subpassLoad(geometry_depth).x), 1.0);
        break;
        default:
        present_color = vec4(0.0, 0.0, 0.0, 1.0);
    }
}
