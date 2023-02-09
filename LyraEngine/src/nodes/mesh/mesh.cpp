#include <nodes/mesh/mesh.h>

namespace lyra {

// mesh
Mesh::Mesh(
	const char* path, 
	Script* script,
	const char* name,
	Spatial* parent,
	const bool& visible,
	const uint32& tag,
	const Transform& transform
) : Spatial(nullptr, name, parent, visible, tag, transform) {
	create_mesh(util::load_model(path));
}

Mesh::Mesh(
	const std::vector <Vertex>& vertices, 
	const std::vector <uint32>& indices, 
	Script* script,
	const char* name,
	Spatial* parent,
	const bool& visible,
	const uint32& tag,
	const Transform& transform
) : Spatial(nullptr, name, parent, visible, tag, transform), m_vertices(vertices), m_indices(indices) { }

void Mesh::create_mesh(const util::LoadedModel& loaded) {
	// this is, as far as I know, veeeeeery inefficient, but I lack the knowlege to make it better, I don't even understand what is going on
	// @todo make some sort of application that loads models into a text file that this engine can read muuuuuuuuuuuch faster and easier but for now, I'll stick to this

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
			// calculate UV coordinates
			vertex.uvw = {
				loaded.vertices.texcoords.at(2 * loaded.shapes.at(s).mesh.indices.at(i).texcoord_index + 0),
				1.0f - loaded.vertices.texcoords.at(2 * loaded.shapes.at(s).mesh.indices.at(i).texcoord_index + 1),
				1.0f
			};

			// add the vertex to the list
			m_vertices.push_back(vertex);
			m_indices.push_back(static_cast<uint32>(m_indices.size()));
		}
	}
}

} // namespace lyra
