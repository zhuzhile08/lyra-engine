#include <nodes/mesh/mesh.h>

#include <core/defines.h>

namespace lyra {

// vertex
const VkVertexInputBindingDescription Mesh::Vertex::get_binding_description() noexcept {
	return {
		0,
		sizeof(Vertex),
		VK_VERTEX_INPUT_RATE_VERTEX
	};
}

const std::array<VkVertexInputAttributeDescription, 4> Mesh::Vertex::get_attribute_descriptions() noexcept {
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

// mesh
void Mesh::create(const char* path, const uint16 index) {
	Logger::log_info("Creating Mesh... ");

	create_mesh(load_model(path), index);

	Logger::log_info("Successfully created mesh at ", get_address(this), "!", Logger::end_l());
}

void Mesh::create(const std::vector <Vertex> vertices, const std::vector <uint32> indices) {
	_vertices = vertices;
	_indices = indices;

	Logger::log_info("Successfully created mesh at ", get_address(this), "!", Logger::end_l());
}

void Mesh::create_mesh(const non_access::LoadedModel loaded, const uint16 index) {
	// this is, as far as I know, veeeeeery inefficient, but I lack the knowlege to make it better, I don't even understand what is going on
	// @todo make some sort of application that loads models into a text file that this engine can read muuuuuuuuuuuch faster and easier but for now, I'll stick to this

	switch (index) {

	case UINT16_MAX: // default option: load everything
		// loop over all the shapes
		for (uint32 s = 0; s < loaded.shapes.size(); s++) {
			// loop over all the polygons
			for (uint32 i = 0; i < loaded.shapes.at(s).mesh.indices.size(); i++) {
				Vertex vertex;

				// calculate positions
				vertex.pos = {
					loaded.vertices.vertices.at(3 * loaded.shapes.at(s).mesh.indices.at(i).vertex_index + 0),
					loaded.vertices.vertices.at(3 * loaded.shapes.at(s).mesh.indices.at(i).vertex_index + 1),
					loaded.vertices.vertices.at(3 * loaded.shapes.at(s).mesh.indices.at(i).vertex_index + 2)
				};
				// calculate normals
				vertex.normal = {
					loaded.vertices.normals.at(3 * loaded.shapes.at(s).mesh.indices.at(i).normal_index + 0),
					loaded.vertices.normals.at(3 * loaded.shapes.at(s).mesh.indices.at(i).normal_index + 1),
					loaded.vertices.normals.at(3 * loaded.shapes.at(s).mesh.indices.at(i).normal_index + 2)
				};
				// calcualte vertex color
				vertex.color = {
					loaded.vertices.colors.at(3 * loaded.shapes.at(s).mesh.indices.at(i).vertex_index + 0),
					loaded.vertices.colors.at(3 * loaded.shapes.at(s).mesh.indices.at(i).vertex_index + 1),
					loaded.vertices.colors.at(3 * loaded.shapes.at(s).mesh.indices.at(i).vertex_index + 2)
				};
				// calculate UV coordinates
				vertex.uv = {
					loaded.vertices.texcoords.at(2 * loaded.shapes.at(s).mesh.indices.at(i).texcoord_index + 0),
					1.0f - loaded.vertices.texcoords.at(2 * loaded.shapes.at(s).mesh.indices.at(i).texcoord_index + 1)
				};

				// add the vertex to the list
				_vertices.push_back(vertex);
				_indices.push_back(static_cast<uint32>(_indices.size()));
			}
		} 

		break;

	default: // funnily enough, this is not the default option
		// loop over all the polygons
		for (uint32 i = 0; i < loaded.shapes.at(index).mesh.indices.size(); i++) {
			Vertex vertex;

			// calculate positions
			vertex.pos = {
				loaded.vertices.vertices.at(3 * loaded.shapes.at(index).mesh.indices.at(i).vertex_index + 0),
				loaded.vertices.vertices.at(3 * loaded.shapes.at(index).mesh.indices.at(i).vertex_index + 1),
				loaded.vertices.vertices.at(3 * loaded.shapes.at(index).mesh.indices.at(i).vertex_index + 2)
			};
			// calculate normals
			vertex.normal = {
				loaded.vertices.normals.at(3 * loaded.shapes.at(index).mesh.indices.at(i).normal_index + 0),
				loaded.vertices.normals.at(3 * loaded.shapes.at(index).mesh.indices.at(i).normal_index + 1),
				loaded.vertices.normals.at(3 * loaded.shapes.at(index).mesh.indices.at(i).normal_index + 2)
			};
			// calcualte vertex color
			vertex.color = {
				loaded.vertices.colors.at(3 * loaded.shapes.at(index).mesh.indices.at(i).vertex_index + 0),
				loaded.vertices.colors.at(3 * loaded.shapes.at(index).mesh.indices.at(i).vertex_index + 1),
				loaded.vertices.colors.at(3 * loaded.shapes.at(index).mesh.indices.at(i).vertex_index + 2)
			};
			// calculate UV coordinates
			vertex.uv = {
				loaded.vertices.texcoords.at(2 * loaded.shapes.at(index).mesh.indices.at(i).texcoord_index + 0),
				1.0f - loaded.vertices.texcoords.at(2 * loaded.shapes.at(index).mesh.indices.at(i).texcoord_index + 1)
			};

			// add the vertex to the list
			_vertices.push_back(vertex);
			_indices.push_back(static_cast<uint32>(_indices.size()));
		}

		break;

	}
}

} // namespace lyra
