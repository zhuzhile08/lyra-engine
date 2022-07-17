#include <components/mesh/mesh.h>

namespace lyra {

Mesh::Mesh() { }

void Mesh::destroy() noexcept {
	this->~Mesh();
}

void Mesh::create(const char* path, const uint16 index) {
	Logger::log_info("Creating Mesh... ");

	create_mesh(load_model(path), index);

	create_vertex_buffer();
	create_index_buffer();

	Logger::log_info("Successfully created mesh at ", get_address(this), "!", Logger::end_l());
}

void Mesh::create(const std::vector <Vertex> vertices, const std::vector <uint32> indices) {
	_vertices = vertices;
	_indices = indices;

	create_vertex_buffer();
	create_index_buffer();

	Logger::log_info("Successfully created mesh at ", get_address(this), "!", Logger::end_l());
}

void Mesh::bind(Camera* const camera) noexcept {
	camera->add_to_draw_queue(FUNC_PTR(
		vkCmdBindPipeline(
			Application::context()->commandBuffers()->commandBuffer(Application::context()->currentCommandBuffer())->commandBuffer, 
			_material->pipeline()->bindPoint(), _material->pipeline()->pipeline()
		);
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(Application::context()->commandBuffers()->commandBuffer(Application::context()->currentCommandBuffer())->commandBuffer, 0, 1, &_vertexBuffer.buffer(), offsets);
		vkCmdBindIndexBuffer(Application::context()->commandBuffers()->commandBuffer(Application::context()->currentCommandBuffer())->commandBuffer, _indexBuffer.buffer(), 0, VK_INDEX_TYPE_UINT32);
		vkCmdBindDescriptorSets(
			Application::context()->commandBuffers()->commandBuffer(Application::context()->currentCommandBuffer())->commandBuffer, 
			_material->pipeline()->bindPoint(), _material->pipeline()->layout(), 0, 1, _material->descriptor()->get_ptr(), 0, nullptr
		);
		vkCmdDrawIndexed(Application::context()->commandBuffers()->commandBuffer(Application::context()->currentCommandBuffer())->commandBuffer, static_cast<uint32>(_indices.size()), 1, 0, 0, 0);
		));
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
					loaded.vertices.vertices[3 * loaded.shapes.at(s).mesh.indices.at(i).vertex_index + 0],
					loaded.vertices.vertices[3 * loaded.shapes.at(s).mesh.indices.at(i).vertex_index + 1],
					loaded.vertices.vertices[3 * loaded.shapes.at(s).mesh.indices.at(i).vertex_index + 2]
				};
				// calculate normals
				vertex.normal = {
					loaded.vertices.normals[3 * loaded.shapes.at(s).mesh.indices.at(i).normal_index + 0],
					loaded.vertices.normals[3 * loaded.shapes.at(s).mesh.indices.at(i).normal_index + 1],
					loaded.vertices.normals[3 * loaded.shapes.at(s).mesh.indices.at(i).normal_index + 2]
				};
				// calculate UV coordinates
				vertex.uv = {
					loaded.vertices.texcoords[2 * loaded.shapes.at(s).mesh.indices.at(i).texcoord_index + 0],
					loaded.vertices.texcoords[2 * loaded.shapes.at(s).mesh.indices.at(i).texcoord_index + 1]
				};
				// calcualte vertex color
				vertex.color = {
					loaded.vertices.colors[3 * loaded.shapes.at(s).mesh.indices.at(i).vertex_index + 0],
					loaded.vertices.colors[3 * loaded.shapes.at(s).mesh.indices.at(i).vertex_index + 1],
					loaded.vertices.colors[3 * loaded.shapes.at(s).mesh.indices.at(i).vertex_index + 2]
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
				loaded.vertices.vertices[3 * loaded.shapes.at(index).mesh.indices.at(i).vertex_index + 0],
				loaded.vertices.vertices[3 * loaded.shapes.at(index).mesh.indices.at(i).vertex_index + 1],
				loaded.vertices.vertices[3 * loaded.shapes.at(index).mesh.indices.at(i).vertex_index + 2]
			};
			// calculate normals
			vertex.normal = {
				loaded.vertices.normals[3 * loaded.shapes.at(index).mesh.indices.at(i).normal_index + 0],
				loaded.vertices.normals[3 * loaded.shapes.at(index).mesh.indices.at(i).normal_index + 1],
				loaded.vertices.normals[3 * loaded.shapes.at(index).mesh.indices.at(i).normal_index + 2]
			};
			// calculate UV coordinates
			vertex.uv = {
				loaded.vertices.texcoords[2 * loaded.shapes.at(index).mesh.indices.at(i).texcoord_index + 0],
				loaded.vertices.texcoords[2 * loaded.shapes.at(index).mesh.indices.at(i).texcoord_index + 1]
			};
			// calcualte vertex color
			vertex.color = {
				loaded.vertices.colors[3 * loaded.shapes.at(index).mesh.indices.at(i).vertex_index + 0],
				loaded.vertices.colors[3 * loaded.shapes.at(index).mesh.indices.at(i).vertex_index + 1],
				loaded.vertices.colors[3 * loaded.shapes.at(index).mesh.indices.at(i).vertex_index + 2]
			};

			// add the vertex to the list
			_vertices.push_back(vertex);
			_indices.push_back(static_cast<uint32>(_indices.size()));
		}

		break;

	}
}

void Mesh::create_vertex_buffer() {
	// create the staging buffer
	VulkanGPUBuffer stagingBuffer;
	stagingBuffer.create(Application::context()->device(), sizeof(_vertices[0]) * _vertices.size(), VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);

	stagingBuffer.copy_data(_vertices.data());

	// create the vertex buffer
	_vertexBuffer.create(Application::context()->device(), sizeof(_vertices[0]) * _vertices.size(), VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);

	// copy the buffer
	_vertexBuffer.copy(Application::context()->commandPool(), &stagingBuffer);
}

void Mesh::create_index_buffer() {
	// create the staging buffer
	VulkanGPUBuffer stagingBuffer;
	stagingBuffer.create(Application::context()->device(), sizeof(_indices[0]) * _indices.size(), VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);

	stagingBuffer.copy_data(_indices.data());

	// create the vertex buffer
	_indexBuffer.create(Application::context()->device(), sizeof(_indices[0]) * _indices.size(), VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);

	// copy the buffer
	_indexBuffer.copy(Application::context()->commandPool(), &stagingBuffer);
}

} // namespace lyra