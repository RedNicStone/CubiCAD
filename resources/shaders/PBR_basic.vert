#version 450

layout(set = 0, binding = 0) uniform SceneInfo {
    mat4 view;  // view matrix
    mat4 proj;  // projection matrix

    uint nFrame;  // frame ID
    uint frameTime;  // frame time in ns
    uint selectedID;  // currently selected object
    uint hoveredID;  // currently hovered object
} scene_info;

layout(location = 0) in vec3 vert_pos;
layout(location = 1) in vec2 vert_uv;
layout(location = 2) in uint instance_id;
layout(location = 3) in mat4 instance_model;

layout(location = 0) out uint fragment_instance_id;
layout(location = 1) out vec2 fragment_uv;

void main() {
    gl_Position = scene_info.proj * scene_info.view * instance_model * vec4(vert_pos, 1.0);

    fragment_instance_id = instance_id;
    fragment_uv = vert_uv;
}
