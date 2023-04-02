#include <EntitySystem/Mesh.h>

namespace lyra {

// mesh
Mesh::Mesh(std::string_view path) {
	// this is, as far as I know, veeeeeery inefficient, but I lack the knowlege to make it better, I don't even understand what is going on
	// @todo make some sort of application that loads models into a text file that this engine can read muuuuuuuuuuuch faster and easier but for now, I'll stick to this

	auto loaded = util::load_model(path);

	// loop over all the shapes
	for (const auto& shape : loaded.shapes) {
		// loop over all the polygons
		for (const auto& index : shape.mesh.indices) {
			Vertex vertex;

			// calculate positions
			vertex.pos = {
				loaded.vertices.vertices.at(3 * index.vertex_index + 0),
				loaded.vertices.vertices.at(3 * index.vertex_index + 1),
				loaded.vertices.vertices.at(3 * index.vertex_index + 2)
			};
			// calculate normals
			vertex.normal = {
				loaded.vertices.normals.at(3 * index.normal_index + 0),
				loaded.vertices.normals.at(3 * index.normal_index + 1),
				loaded.vertices.normals.at(3 * index.normal_index + 2)
			};
			// calculate UV coordinates
			vertex.uvw = {
				loaded.vertices.texcoords.at(2 * index.texcoord_index + 0),
				1.0f - loaded.vertices.texcoords.at(2 * index.texcoord_index + 1),
				1.0f
			};

			// add the vertex to the list
			m_vertices.push_back(vertex);
			m_indices.push_back(static_cast<uint32>(m_indices.size()));
		}
	}
}

Mesh::Mesh(const std::vector <Vertex>& vertices, const std::vector <uint32>& indices)
	 : m_vertices(vertices), m_indices(indices) { }

} // namespace lyra
