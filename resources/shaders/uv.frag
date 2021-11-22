#version 450

layout(location = 0) in vec2 fragUV;

layout(location = 0) out vec4 outColor;

layout(binding = 0) uniform FrameInfoObject {
	uint frameCount;
	uint frameID;
	uint frameTime;
	uint time;
} frameInfo;

void main() {
	outColor = vec4(fragUV, frameInfo.frameCount % 255, 0);
}
