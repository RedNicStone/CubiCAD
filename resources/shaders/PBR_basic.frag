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
    vec4 diffuse;
} properties;
layout(set = 2, binding = 1) uniform texture2D diffuse;

flat layout(location = 0) in uint vert_instance_id;
layout(location = 1) in vec3 vert_normal;
layout(location = 2) in vec2 vert_uv;
layout(location = 3) in vec3 vert_pos;

layout(location = 0) out vec4 shading_diffuse;
layout(location = 1) out vec4 shading_pos;
layout(location = 2) out vec2 shading_normal;

layout(location = 3) out uint backbuffer_instance_id;

layout (constant_id = 0) const int USE_DIFFUSE_TEX = 0;

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
    vec4 color;
    if (USE_DIFFUSE_TEX == 2)
        color = texture(sampler2D(diffuse, samp), vert_uv);
    else
        color = properties.diffuse;
    float brightness = (int(vert_instance_id == scene_info.selectedID) * 2 + int(vert_instance_id == scene_info.hoveredID)) / 3.0f;
    shading_diffuse = vec4(color + vec4(vec3(brightness) / 3, 1.0f));
    shading_pos = vec4(vert_pos, 0.0);
    shading_normal = azimuthalEAEncode(normalize(vert_normal) * 0.5 + 0.5);
    backbuffer_instance_id = vert_instance_id;
}
