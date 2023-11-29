#version 450
#extension GL_EXT_nonuniform_qualifier : enable

layout(set = 0, binding = 2) uniform sampler2D metallic;
layout(set = 0, binding = 2) uniform sampler2D specular;
layout(set = 0, binding = 4) uniform sampler2D emission;
layout(set = 0, binding = 5) uniform sampler2D occlusionMap;

layout(set = 0, binding = 6) uniform MaterialDataFrag {
	vec4 albedoColor;
	vec4 emissionColor;
	uint metallic;
	uint roughness;
	uint specular;
	bool emissionEnabled;
	uint emissionEnergy;
	uint occlusionMapValue;
} matDatF;

layout(set = 0, binding = 7) uniform sampler2D albedo[];

layout(location = 0) in vec3 inColor;
layout(location = 1) in vec3 inUVWCoord;

layout(location = 0) out vec4 outColor;

void main() {
	// outColor = texture(albedo[uint(inUVWCoord.z)], inUVWCoord.xy);
	// outColor = texture(albedo[0], inUVWCoord.xy);
	outColor = vec4(inUVWCoord, 1.0);
}
