#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inColor;
layout(location = 3) in vec3 inUVW;

layout(location = 0) out vec3 outColor;
layout(location = 1) out vec3 outTexCoord;

layout(push_constant) uniform TransformData {
	mat4 model;
	mat4 proj;
	mat4 mesh;
} transform;

layout(set = 0, binding = 0) uniform sampler2D normalMap;
layout(set = 0, binding = 1) uniform sampler2D displacementMap;

void main() {
	gl_Position = transform.proj * transform.model * transform.mesh * vec4(inPosition, 1.0);
	outColor = inColor;
	outTexCoord = inUVW; 
}
