#version 450

layout(set = 0, binding = 0) uniform SceneInfo {
    mat4 view;// view matrix
    mat4 proj;// projection matrix

    uint nFrame;// frame ID
    uint frameTime;// frame time in ns
    uint selectedID;// currently selected object
    uint hoveredID;// currently hovered object
} scene_info;

struct InstanceBufferData {
    mat4 model;
    vec3 bbox1;
    uint objectID;
    vec3 bbox2;
    uint batchID;
};

layout(set = 0, binding = 1) readonly buffer InstanceBuffer {
    InstanceBufferData instances[];
} instanceBuffer;

layout(location = 0) in vec3 vert_pos;
layout(location = 1) in vec3 vert_normal;
layout(location = 2) in vec2 vert_uv;
layout(location = 3) in uint instance_id;

layout(location = 0) out uint fragment_instance_id;
layout(location = 1) out vec3 fragment_normal;
layout(location = 2) out vec2 fragment_uv;
layout(location = 3) out vec3 fragment_pos;

void main() {
    mat4 instance_model = instanceBuffer.instances[instance_id].model;
    vec4 absPos = scene_info.view * instance_model * vec4(vert_pos, 1.0);
    gl_Position = scene_info.proj * absPos;

    fragment_instance_id = instanceBuffer.instances[instance_id].objectID;
    mat3 normalMatrix = transpose(inverse(mat3(scene_info.view * instance_model)));
    fragment_normal = normalMatrix * vert_normal;
    fragment_uv = vert_uv;
    fragment_pos = absPos.xyz;
}
