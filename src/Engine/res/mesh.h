/*************************
 * @file mesh.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 * 
 * @brief a file with geometry and mesh related structs
 * @brief has a struct for vertexes, indexes, uniform buffers and meshes
 * 
 * @date 2022-02-03
 * 
 * @copyright Copyright (c) 2022 Zhile Zhu
 *************************/

#pragma once

#include <core/defines.h>
#include <noud.h>
#include <res/loaders/load_model.h>

#include <rendering/renderer.h>
#include <rendering/vulkan/GPU_buffer.h>

#define GLM_FORCE_RADIANS

#include <vector>
#include <array>
#include <glm.hpp>
#include <vulkan/vulkan.h>

namespace lyra {

/**
 * @brief A mesh struct containing vertices, indecies and vertex and index buffers
 */
class Mesh : noud::Node {
public:
    /**
     * @brief Vertex struct with Vulkan input information
     */
    struct              Vertex {
        glm::vec3                                               pos;
        glm::vec3                                               normal;
        glm::vec3                                               color;
        glm::vec2                                               uv;

        Vertex();

        /**
         * @brief construct a new Vertex object
         * 
         * @param pos the new position
         * @param normal vertex normals
         * @param color the new color
         */
        Vertex(glm::vec3 pos, glm::vec3 normal, glm::vec2 uv, glm::vec3 color = {0, 0, 0});

        /**
         * @brief returns a static vertex binding
         * 
         * @return VkVertexInputBindingDescription 
         */
        static VkVertexInputBindingDescription                  get_binding_description();

        /**
         * @brief returns a static vertex input attribute
         * 
         * @return std::array<VkVertexInputAttributeDescription, 4> 
         */
        static std::array<VkVertexInputAttributeDescription, 4> get_attribute_descriptions();
    };

private:
    struct              Variables {
        std::vector <Vertex>    vertices;
        std::vector <uint16>    indices;

        VulkanGPUBuffer         vertexBuffer;
        VulkanGPUBuffer         indexBuffer;
    };

public:
    Mesh();
    
    /**
     * @brief destroy the mesh
     */
    void                destroy();

    /**
     * @brief construct a new mesh loaded from a .obj file
     * 
     * @param device a Vulkan device to create the buffers
     * @param commandBuffer a Vulkan command buffer to create and record the buffers
     * @param path path of the model to load
     * @param parent parent node
     * @param name name of the node
     */
    void                create(VulkanDevice device, VulkanCommandPool commandPool, const non_access::LoadedModel loaded, uint16 index = 0, 
        noud::Node* parent = nullptr, const std::string name = "mesh");
    /**
     * @brief construct a new mesh with a custom model
     * @brief the vertices and indecies are user defined, which makes it perfect for generated meshes
     * 
     * @param device a Vulkan device to create the buffers
     * @param commandBuffer a Vulkan command buffer to create and record the buffers
     * @param vertices the new vertices
     * @param indices the new indices
     * @param parent parent node
     * @param name name of the node
     */
    void                create(VulkanDevice device, VulkanCommandPool commandPool, const std::vector <Vertex> vertices, const std::vector <uint16> indices, 
        noud::Node* parent = nullptr, const std::string name = "mesh");

    /**
     * @brief create a mesh from a already loaded .obj file
     * 
     * @param load an already loaded model
     * @param index load the model with the following index if a file has more than just one object. Will load everything on default
     */
    void                create_mesh(const non_access::LoadedModel loaded, const uint16 index = UINT16_MAX);

    /**
     * add the mesh and its buffers to the renderer draw queue
     * 
     * @param renderer renderer to add the draw call to
     * @param index index of the command buffer to record the draw and bind commands
     */
    void                draw(Renderer renderer, int index);

    /**
     * @brief get all variables
     * 
     * @return Variables 
     */
    Variables           get() const;

private:
    Variables           var;

    VulkanDevice*       device;
    VulkanCommandPool*  commandPool;

    /**
     * @brief create a vertex buffer
     */
    void                create_vertex_buffer();

    /**
     * @brief create a index buffer
     */
    void                create_index_buffer();
};

} // namespace lyra
