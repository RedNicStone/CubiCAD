#version 450

layout(location = 0) in vec2 frag_uv;

layout(location = 0) out vec4 present_color;

layout(binding = 0) uniform FrameInfoObject {
    uint frameCount;
    uint frameID;
    uint frameTime;
    uint time;
} frameInfo;

void main() {
    present_color = vec4(frag_uv, (frameInfo.frameCount % 4095) / 4095.0, 0);
}
