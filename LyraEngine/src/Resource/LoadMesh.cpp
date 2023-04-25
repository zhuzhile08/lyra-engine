#include <Resource/LoadMesh.h>

#include <Common/Logger.h>

namespace lyra {

namespace util {

LoadedMesh load_mesh(std::string_view path) {
	// TOL model data
	LoadedMesh::TOLMesh load;
	// warning and errors
	std::string error, warning;

	// load the model
	tinyobj::LoadObj(&load.vertices, &load.shapes, nullptr, &warning, &error, path.data());

	// check if there are warnings
	if (!warning.empty()) log().warning("A problem occurred while loading a material: ", warning);
	// check if there are errors
	if (!error.empty()) log().error("An error occurred while loading a material: ", error);

	// convert to engine-readable data
	LoadedMesh mesh;
	mesh.path = path;
	// loop over all the shapes
	for (const auto& shape : load.shapes) {
		// loop over all the polygons
		for (const auto& index : shape.mesh.indices) {
			LoadedMesh::Vertex vertex;

			// calculate positions
			vertex.pos = {
				load.vertices.vertices.at(3 * index.vertex_index + 0),
				load.vertices.vertices.at(3 * index.vertex_index + 1),
				load.vertices.vertices.at(3 * index.vertex_index + 2)
			};
			// calculate normals
			vertex.normal = {
				load.vertices.normals.at(3 * index.normal_index + 0),
				load.vertices.normals.at(3 * index.normal_index + 1),
				load.vertices.normals.at(3 * index.normal_index + 2)
			};
			// calculate UV coordinates
			vertex.uvw = {
				load.vertices.texcoords.at(2 * index.texcoord_index + 0),
				1.0f - load.vertices.texcoords.at(2 * index.texcoord_index + 1),
				1.0f
			};

			// add the vertex to the list
			mesh.vertices.push_back(vertex);
			mesh.indices.push_back(static_cast<uint32>(mesh.indices.size()));
		}
	}

	// this is, as far as I know, veeeeeery inefficient, but I lack the knowlege to make it better, I don't even understand what is going on
	/// @todo make some sort of application that loads models into a text file that this engine can read muuuuuuuuuuuch faster and easier but for now, I'll stick to this

	return mesh;
}

} // namespace util

} // namespace lyra
