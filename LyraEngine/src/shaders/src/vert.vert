#version 450

layout(set = 0, binding = 0) uniform CameraData {
	mat4 model;
	mat4 proj;	
} ubo;

layout(set = 1, binding = 1) uniform MaterialDataVert {
	uint normalMapValue;
	int displacementMapValue;
} matDatV;

layout(set = 1, binding = 3) uniform sampler2D normalMap;
layout(set = 1, binding = 4) uniform sampler2D displacementMap;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inColor;
layout(location = 3) in vec3 inUVW;

layout(location = 0) out vec3 outColor;
layout(location = 1) out vec3 outTexCoord;

void main() {
	gl_Position = ubo.proj * ubo.model* vec4(inPosition, 1.0f);
	outColor = inColor; 
	outTexCoord = inUVW; 
}
