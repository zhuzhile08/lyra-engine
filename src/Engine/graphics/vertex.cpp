#include <graphics/vertex.h>

namespace lyra {

Vertex::Vertex() { }

Vertex::Vertex(glm::vec3 pos, glm::vec3 normal, glm::vec2 uv, glm::vec3 color) : pos(pos), normal(normal), color(color), uv(uv) { }

VkVertexInputBindingDescription Vertex::get_binding_description() noexcept {
	return {
		0,
		sizeof(Vertex),
		VK_VERTEX_INPUT_RATE_VERTEX
	};
}

std::array<VkVertexInputAttributeDescription, 4> Vertex::get_attribute_descriptions() noexcept {
	return {
		{{
			0,
			0,
			VK_FORMAT_R32G32B32_SFLOAT,
			offsetof(Vertex, pos)
		},
		{
			1,
			0,
			VK_FORMAT_R32G32B32_SFLOAT,
			offsetof(Vertex, normal)
		},
		{
			2,
			0,
			VK_FORMAT_R32G32B32_SFLOAT,
			offsetof(Vertex, color)
		},
		{
			3,
			0,
			VK_FORMAT_R32G32B32_SFLOAT,
			offsetof(Vertex, uv)
		}}
	};
}

} // namespace lyra