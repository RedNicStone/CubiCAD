#version 450

layout(set = 0, binding = 0) uniform SceneInfo {
    mat4 view;// view matrix
    mat4 proj;// projection matrix

    uint nFrame;// frame ID
    uint frameTime;// frame time in ns
    uint selectedID;// currently selected object
    uint hoveredID;// currently hovered object
} scene_info;
layout(set = 1, binding = 0) uniform sampler samp;
layout(set = 2, binding = 0) uniform ShaderProperties {
    vec4 color;
} properties;
layout(set = 2, binding = 1) uniform texture2D color;

flat layout(location = 0) in uint vert_instance_id;
layout(location = 1) in vec2 vert_uv;

layout(location = 0) out vec4 shading_diffuse;
layout(location = 1) out vec4 shading_pos;
layout(location = 2) out vec2 shading_normal;

layout(location = 3) out uint backbuffer_instance_id;

float xor(float an) {
    vec2 p = 256.0 * vert_uv;

    float x = 0.0;
    for (int i=0; i<7; i++)
    {
        vec2 a = floor(p);
        vec2 b = fract(p);

        x += mod(a.x + a.y, 2.0) *

        // the following line implements the smooth xor
        mix(1.0, 1.5*pow(4.0*(1.0-b.x)*b.x*(1.0-b.y)*b.y, 0.25), an);

        p /= 2.0;
        x /= 2.0;
    }

    return x;
}

void main() {
    vec4 color = texture(sampler2D(color, samp), vert_uv);
    float brightness = (int(vert_instance_id == scene_info.selectedID) * 2 + int(vert_instance_id == scene_info.hoveredID)) / 3.0f;
    shading_diffuse = vec4(color + vec4(vec3(brightness) / 3, 1.0f));
    backbuffer_instance_id = vert_instance_id;
}
