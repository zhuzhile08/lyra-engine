#version 450

layout(binding = 1) uniform sampler2D albedo;

layout(location = 0) in vec3 inColor;
layout(location = 1) in vec2 inUVCoord;

layout(location = 0) out vec4 outColor;

void main() {
	outColor = texture(albedo, inUVCoord);
	//outColor = vec4(inColor, 1.0);
}
