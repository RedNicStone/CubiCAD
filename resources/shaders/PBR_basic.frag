#version 450

layout(set = 0, binding = 0) uniform SceneInfo {
    mat4 view;  // view matrix
    mat4 proj;  // projection matrix

    uint nFrame;  // frame ID
    uint frameTime;  // frame time in ns
} scene_info;

flat layout(location = 1) in uint instance_id;

layout(location = 0) out vec4 pixel_color;

void main() {
    pixel_color = vec4(instance_id, scene_info.nFrame, scene_info.frameTime, 0);
}
