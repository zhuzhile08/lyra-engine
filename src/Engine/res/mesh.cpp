#include <res/mesh.h>

namespace lyra {

// vertex
Mesh::Vertex::Vertex() { }

Mesh::Vertex::Vertex(glm::vec3 pos, glm::vec3 normal, glm::vec3 color) : pos(pos), normal(normal), color(color), uv(uv) { }

VkVertexInputBindingDescription Mesh::Vertex::get_binding_description() {
    return {
        0,
        sizeof(Mesh::Vertex),
        VK_VERTEX_INPUT_RATE_VERTEX
    };
}

std::array<VkVertexInputAttributeDescription, 4> Mesh::Vertex::get_attribute_descriptions() {
    return {
        {{
            0,
            0,
            VK_FORMAT_R32G32_SFLOAT,
            offsetof(Vertex, pos)
        },
        {
            0,
            1,
            VK_FORMAT_R32G32B32_SFLOAT,
            offsetof(Vertex, normal)
        },
        {
            0,
            2,
            VK_FORMAT_R32G32B32_SFLOAT,
            offsetof(Vertex, color)
        },
        {
            0,
            3,
            VK_FORMAT_R32G32B32_SFLOAT,
            offsetof(Vertex, uv)
        }}
    };
}

// mesh
Mesh::Mesh() { }

void Mesh::destroy() {
    var.vertexBuffer.destroy();
    var.indexBuffer.destroy();

    delete device;
    delete commandPool;

    LOG_INFO("Succesfully destroyed mesh!")
}

void Mesh::create(VulkanDevice device, VulkanCommandPool commandPool, const non_access::LoadedModel loaded, uint16 index, 
    noud::Node* parent, const std::string name) {
    (parent, name);

    this->commandPool = &commandPool;
    this->device = &device;

    create_mesh(loaded, index);

    create_vertex_buffer();
    create_index_buffer();

    LOG_INFO("Succesfully created mesh at ", GET_ADDRESS(this), "!", END_L)
}

void Mesh::create(VulkanDevice device, VulkanCommandPool commandPool, const std::vector <Vertex> vertices, const std::vector <uint16> indices, 
    noud::Node* parent, const std::string name) {
    (parent, name);

    this->commandPool = &commandPool;
    this->device = &device;
    var.vertices = vertices;
    var.indices = indices;

    create_vertex_buffer();
    create_index_buffer();

    LOG_INFO("Succesfully created mesh at ", GET_ADDRESS(this), "!", END_L)
}

void Mesh::create_mesh(const non_access::LoadedModel loaded, uint16 index) {
    // this is, as far as I know, veeeeeery inefficient, but I lack the knowlege to make it better, I don't even understand what is going on

    switch (index) {

    case UINT16_MAX: // default option: load everything
        // loop over all the shapes
        for (size_t shapeIndex = 0; shapeIndex < loaded.shapes.size(); shapeIndex++) {
            // loop over all the polygons
            size_t index_offset = 0;
            for (size_t polyIndex = 0; polyIndex < loaded.shapes[shapeIndex].mesh.num_face_vertices.size(); polyIndex++) {
                // loop over the vertices in the polygon.
                for (size_t vertexIndex = 0; vertexIndex < 3; vertexIndex++) { // the number of vertices in a face is hard coded to three because the tutorial says so
                    tinyobj::index_t idx = loaded.shapes[shapeIndex].mesh.indices[index_offset + vertexIndex];

                    // calculate positions
                    tinyobj::real_t vertexPositions[3] = {
                        loaded.vertices.vertices[3 * idx.vertex_index + 0],
                        loaded.vertices.vertices[3 * idx.vertex_index + 1],
                        loaded.vertices.vertices[3 * idx.vertex_index + 2]
                    };
                    // calculate normals
                    tinyobj::real_t normals[3] = {
                        loaded.vertices.normals[3 * idx.normal_index + 0],
                        loaded.vertices.normals[3 * idx.normal_index + 1],
                        loaded.vertices.normals[3 * idx.normal_index + 2]
                    }; 

                    // the vertex
                    Vertex vertex = Vertex(
                        {vertexPositions[0], vertexPositions[1], vertexPositions[2]}, // position
                        {normals[0], normals[1], normals[2]}, // normals
                        vertex.normal // color are now set to the normal position for testing purposes
                    );

                    // add the vertex to the list
                    var.vertices.push_back(vertex);
                }

                index_offset += 3;
            }
        }
        break;
    
    default: // funnily enough, this is not the default option
        // loop over all the polygons
        size_t index_offset = 0;
        for (size_t polyIndex = 0; polyIndex < loaded.shapes[index].mesh.num_face_vertices.size(); polyIndex++) {
            // loop over the vertices in the polygon.
            for (size_t vertexIndex = 0; vertexIndex < 3; vertexIndex++) { // the number of vertices in a face is hard coded to three because the tutorial says so
                tinyobj::index_t idx = loaded.shapes[index].mesh.indices[index_offset + vertexIndex];

                // calculate positions
                tinyobj::real_t vertexPositions[3] = {
                    loaded.vertices.vertices[3 * idx.vertex_index + 0],
                    loaded.vertices.vertices[3 * idx.vertex_index + 1],
                    loaded.vertices.vertices[3 * idx.vertex_index + 2]
                };
                // calculate normals
                tinyobj::real_t normals[3] = {
                    loaded.vertices.normals[3 * idx.normal_index + 0],
                    loaded.vertices.normals[3 * idx.normal_index + 1],
                    loaded.vertices.normals[3 * idx.normal_index + 2]
                }; 

                // the vertex
                Vertex vertex = Vertex(
                    {vertexPositions[0], vertexPositions[1], vertexPositions[2]}, // position
                    {normals[0], normals[1], normals[2]}, // normals
                    vertex.normal // color are now set to the normal position for testing purposes
                );

                // add the vertex to the list
                var.vertices.push_back(vertex);
            }

            index_offset += 3;
        }
        break;

    }
}

void Mesh::create_vertex_buffer() {
    // create the staging buffer
    VulkanGPUBuffer stagingBuffer;
    stagingBuffer.create(*device, sizeof(var.vertices[0]) * var.vertices.size(), VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);

    void* data;
    stagingBuffer.map_memory(data);
        memcpy(data, var.vertices.data(), (size_t)stagingBuffer.get().size);
    stagingBuffer.unmap_memory();

    // create the vertex buffer
    var.vertexBuffer.create(*device, sizeof(var.vertices[0]) * var.vertices.size(), VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);
    
    // copy the buffer
    var.vertexBuffer.copy(*commandPool, stagingBuffer);
    
    // destroy the staging buffer
    stagingBuffer.destroy();
}

void Mesh::create_index_buffer() {
    // create the staging buffer
    VulkanGPUBuffer stagingBuffer;
    stagingBuffer.create(*device, sizeof(var.indices[0]) * var.indices.size(), VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);

    void* data;
    stagingBuffer.map_memory(data);
        memcpy(data, var.indices.data(), (size_t)stagingBuffer.get().size);
    stagingBuffer.unmap_memory();

    // create the vertex buffer
    var.indexBuffer.create(*device, sizeof(var.indices[0]) * var.indices.size(), VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);
    
    // copy the buffer
    var.indexBuffer.copy(*commandPool, stagingBuffer);
    
    // destroy the staging buffer
    stagingBuffer.destroy();
}

Mesh::Variables Mesh::get() const {
    return var;
};

} // namespace lyra