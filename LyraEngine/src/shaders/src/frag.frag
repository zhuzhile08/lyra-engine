#version 450

layout(set = 1, binding = 2) uniform sampler2D albedo; // 24 - 5 samplers

layout(set = 1, binding = 5) uniform MaterialDataFrag {
	vec4 albedoColor;
	vec4 emissionColor;
	uint metallic;
	uint roughness;
	uint specular;
	bool emissionEnabled;
	uint emissionEnergy;
	uint occlusionMapValue;
} matDatF;

layout(set = 1, binding = 6) uniform sampler2D metallic;
layout(set = 1, binding = 7) uniform sampler2D emission;
layout(set = 1, binding = 8) uniform sampler2D occlusionMap;

layout(location = 0) in vec3 inColor;
layout(location = 1) in vec3 inUVWCoord;

layout(location = 0) out vec4 outColor;

void main() {
	outColor = texture(albedo, inUVWCoord.xy);
	//outColor = vec4(inColor, 1.0);
}
