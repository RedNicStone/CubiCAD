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

float xor(float an) {
    vec2 p = 256.0 * vert_uv;

    float x = 0.0;
    for( int i=0; i<7; i++ )
    {
        vec2 a = floor(p);
        vec2 b = fract(p);

        x += mod( a.x + a.y, 2.0 ) *

        // the following line implements the smooth xor
        mix( 1.0, 1.5*pow(4.0*(1.0-b.x)*b.x*(1.0-b.y)*b.y,0.25), an );

        p /= 2.0;
        x /= 2.0;
    }

    return x;
}

void main() {
    vec3 color = vec3(vert_uv/2, xor(0.5));
    float brightness = (int(vert_instance_id == scene_info.selectedID) * 2 + int(vert_instance_id == scene_info.hoveredID)) / 3.0f;
    pixel_color = vec4(color + vec3(brightness), 0.0f);
    instance_id = vert_instance_id;
}
