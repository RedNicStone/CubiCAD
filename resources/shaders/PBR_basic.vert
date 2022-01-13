#version 450

layout(set = 0, binding = 0) uniform SceneInfo {
    mat4 view;  // view matrix
    mat4 proj;  // projection matrix

    uint nFrame;  // frame ID
    uint frameTime;  // frame time in ns
} scene_info;

layout(location = 0) in vec3 vert_pos;
layout(location = 1) in uint instance_id;
layout(location = 2) in mat4 instance_model;

layout(location = 1) out uint fragment_instance_id;

void main() {
    gl_Position = vec4(vert_pos, 1.0) * scene_info.view * scene_info.proj * instance_model;

    fragment_instance_id = instance_id;
}
