#version 450

layout(binding = 4) uniform FragmentData {
	vec4 albedoColor;
	uint metallic;
	uint roughness;
	uint specular;
	bool m_emissionEnabled;
	vec4 m_emissionColor;
	uint emissionEnergy;
	uint occlusionMapValue;
} fragData;

layout(binding = 5) uniform sampler2D albedo;
layout(binding = 6) uniform sampler2D metallic;
layout(binding = 7) uniform sampler2D emission;
layout(binding = 8) uniform sampler2D occlusion;

layout(location = 0) in vec3 inColor;
layout(location = 1) in vec2 inUVCoord;

layout(location = 0) out vec4 outColor;

void main() {
	outColor = vec4(texture(albedo, inUVCoord).rgb, 1.0);
}
