#include <res/mesh.h>

namespace lyra {

Mesh::Mesh() { }

void Mesh::destroy() noexcept {
	_vertexBuffer.destroy();
	_indexBuffer.destroy();

	delete renderer;

	LOG_INFO("Succesfully destroyed mesh!")
}

void Mesh::create(Renderer renderer, const non_access::LoadedModel loaded, uint16 index,
	noud::Node* parent, const std::string name) {
	(parent, name);

	this->renderer = &renderer;

	create_mesh(loaded, index);

	create_vertex_buffer();
	create_index_buffer();

	LOG_INFO("Succesfully created mesh at ", GET_ADDRESS(this), "!", END_L)
}

void Mesh::create(Renderer renderer, const std::vector <Vertex> vertices, const std::vector <uint16> indices,
	noud::Node* parent, const std::string name) {
	(parent, name);

	this->renderer = &renderer;
	_vertices = vertices;
	_indices = indices;

	create_vertex_buffer();
	create_index_buffer();

	LOG_INFO("Succesfully created mesh at ", GET_ADDRESS(this), "!", END_L)
}

void Mesh::create_mesh(const non_access::LoadedModel loaded, uint16 index) {
	// this is, as far as I know, veeeeeery inefficient, but I lack the knowlege to make it better, I don't even understand what is going on
	// @todo make some sort of application that loads models into a text file that this engine can read muuuuuuuuuuuch faster and easier for now, I'll stick to this

	switch (index) {

	case UINT16_MAX: // default option: load everything
		// loop over all the shapes
		for (const auto& shape : loaded.shapes) {
			// loop over all the polygons
			size_t index_offset = 0;
			for (const auto& index : shape.mesh.indices) {
				// calculate positions
				tinyobj::real_t vertexPositions[3] = {
					loaded.vertices.vertices[3 * index.vertex_index + 0],
					loaded.vertices.vertices[3 * index.vertex_index + 1],
					loaded.vertices.vertices[3 * index.vertex_index + 2]
				};
				// calculate normals
				tinyobj::real_t normals[3] = {
					loaded.vertices.normals[3 * index.normal_index + 0],
					loaded.vertices.normals[3 * index.normal_index + 1],
					loaded.vertices.normals[3 * index.normal_index + 2]
				};

				tinyobj::real_t uv[2] = {
					loaded.vertices.texcoords[2 * index.texcoord_index + 0],
					loaded.vertices.texcoords[2 * index.texcoord_index + 1]
				};

				tinyobj::real_t colors[3] = {
					loaded.vertices.colors[3 * index.vertex_index + 0],
					loaded.vertices.colors[3 * index.vertex_index + 1],
					loaded.vertices.colors[3 * index.vertex_index + 2]
				};

				// the vertex
				Vertex vertex = Vertex(
					{ vertexPositions[0], vertexPositions[1], vertexPositions[2] }, // position
					{ normals[0], normals[1], normals[2] }, // normals
					{ uv[0], 1 - uv[1] }, // texture UV coordinates
					{ colors[0], colors[1], colors[2] } // colors
				);

				// add the vertex to the list
				_vertices.push_back(vertex);
				_indices.push_back(_indices.size());

				index_offset += 3;
			}
		}

		break;

	default: // funnily enough, this is not the default option
		// loop over all the shapes
		for (const auto& shape : loaded.shapes) {
			// loop over all the polygons
			size_t index_offset = 0;
			for (const auto& index : shape.mesh.indices) {
				// calculate positions
				tinyobj::real_t vertexPositions[3] = {
					loaded.vertices.vertices[3 * index.vertex_index + 0],
					loaded.vertices.vertices[3 * index.vertex_index + 1],
					loaded.vertices.vertices[3 * index.vertex_index + 2]
				};
				// calculate normals
				tinyobj::real_t normals[3] = {
					loaded.vertices.normals[3 * index.normal_index + 0],
					loaded.vertices.normals[3 * index.normal_index + 1],
					loaded.vertices.normals[3 * index.normal_index + 2]
				};

				tinyobj::real_t uv[2] = {
					loaded.vertices.texcoords[2 * index.texcoord_index + 0],
					loaded.vertices.texcoords[2 * index.texcoord_index + 1]
				};

				tinyobj::real_t colors[3] = {
					loaded.vertices.colors[3 * index.vertex_index + 0],
					loaded.vertices.colors[3 * index.vertex_index + 1],
					loaded.vertices.colors[3 * index.vertex_index + 2]
				};

				// the vertex
				Vertex vertex = Vertex(
					{ vertexPositions[0], vertexPositions[1], vertexPositions[2] }, // position
					{ normals[0], normals[1], normals[2] }, // normals
					{ uv[0], 1 - uv[1] }, // texture UV coordinates
					{ colors[0], colors[1], colors[2] } // colors
				);

				// add the vertex to the list
				_vertices.push_back(vertex);
				_indices.push_back(_indices.size());

				index_offset += 3;
			}
		}

		break;

	}
}

void Mesh::create_vertex_buffer() {
	// create the staging buffer
	VulkanGPUBuffer stagingBuffer;
	stagingBuffer.create(renderer->device(), sizeof(_vertices[0]) * _vertices.size(), VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);

	stagingBuffer.copy_data(_vertices.data());

	// create the vertex buffer
	_vertexBuffer.create(renderer->device(), sizeof(_vertices[0]) * _vertices.size(), VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);

	// copy the buffer
	_vertexBuffer.copy(renderer->commandPool(), stagingBuffer);

	// destroy the staging buffer
	stagingBuffer.destroy();
}

void Mesh::create_index_buffer() {
	// create the staging buffer
	VulkanGPUBuffer stagingBuffer;
	stagingBuffer.create(renderer->device(), sizeof(_indices[0]) * _indices.size(), VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);

	stagingBuffer.copy_data(_indices.data());

	// create the vertex buffer
	_indexBuffer.create(renderer->device(), sizeof(_indices[0]) * _indices.size(), VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);

	// copy the buffer
	_indexBuffer.copy(renderer->commandPool(), stagingBuffer);

	// destroy the staging buffer
	stagingBuffer.destroy();
}

void Mesh::draw(RenderStage renderStage) noexcept {
	renderStage._bind_queue.add([=]() { renderStage.bind_model(_vertexBuffer.buffer(), _indexBuffer.buffer()); });
	renderStage._draw_queue.add([=]() { renderStage.draw_model(static_cast<uint32>(_indices.size())); });
}

} // namespace lyra