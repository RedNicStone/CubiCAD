#version 450

layout(set = 0, binding = 0) uniform SceneInfo {
    mat4 view;  // view matrix
    mat4 proj;  // projection matrix

    uint nFrame;  // frame ID
    uint frameTime;  // frame time in ns
} scene_info;

flat layout(location = 0) in uint vert_instance_id;
layout(location = 1) in vec2 vert_uv;

layout(location = 0) out vec4 pixel_color;
layout(location = 1) out uint instance_id;

void main() {
    pixel_color = vec4(vert_uv, scene_info.frameTime, 0);
    instance_id = vert_instance_id;
}
