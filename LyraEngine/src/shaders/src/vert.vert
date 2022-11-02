#version 450

layout(binding = 0) uniform CameraData {
	mat4 model;
	mat4 proj;	
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inColor;
layout(location = 3) in vec2 inUV;

layout(location = 0) out vec3 outColor;
layout(location = 1) out vec2 outTexCoord;

void main() {
	gl_Position = ubo.model * ubo.proj * vec4(inPosition, 1.0f);
	outColor = inColor;
	outTexCoord = inUV; 
}
