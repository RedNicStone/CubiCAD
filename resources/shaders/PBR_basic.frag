#version 450

layout(set = 0, binding = 0) uniform SceneInfo {
    mat4 view;  // view matrix
    mat4 proj;  // projection matrix

    uint nFrame;  // frame ID
    uint frameTime;  // frame time in ns
    uint selectedID;  // currently selected object
    uint hoveredID;  // currently hovered object
} scene_info;

flat layout(location = 0) in uint vert_instance_id;
layout(location = 1) in vec2 vert_uv;

layout(location = 0) out vec4 pixel_color;
layout(location = 1) out uint instance_id;

void main() {
    vec3 color = vec3(0.1);
    float brightness = (int(vert_instance_id == scene_info.selectedID) * 2 + int(vert_instance_id == scene_info.hoveredID)) / 3.0f;
    pixel_color = vec4(vec3(vert_uv, 1.0f) + vec3(brightness), 0.0f);
    instance_id = vert_instance_id;
}
