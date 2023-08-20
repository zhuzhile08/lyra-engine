/*************************
 * @file Material.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 *
 * @brief a material system
 *
 * @date 2022-05-27
 *
 * @copyright Copyright (c) 2022
 *************************/

#pragma once

#include <Common/Common.h>

#include <Common/Array.h>
#include <Common/UniquePointer.h>

#include <Resource/LoadMaterial.h>

#include <Common/Node.h>

#include <Graphics/VulkanImpl/GPUBuffer.h>
#include <Graphics/VulkanImpl/DescriptorSystem.h>
#include <Graphics/GraphicsPipelineSystem.h>

namespace lyra {

/**
 * @brief Implementation of a basic material system
 */
class Material {
private:
	// texture with it's path/hash loaded from the resource manager
	struct HashedTexture {
		HashedTexture() = default;
		/**
		 * @brief construct the hashed texture
		 * 
		 * @param p path
		 * @param t texture
		 */
		HashedTexture(std::string_view p, const Texture* t) : path(p), texture(t) { }

		std::string path;
		const Texture* texture;
	};

public:
	// data to send to the vertex shader
	struct MaterialVertexData {
		alignas(8) const uint32 m_normalMapValue;
		alignas(8) const int32 m_displacementMapValue;
	};
	// data to send to the fragment shader
	struct MaterialFragmentData {
		alignas(16) const glm::vec4 m_albedoColor;
		alignas(16) const glm::vec4 m_emissionColor;
		alignas(8) const uint32 m_metallic;
		alignas(8) const uint32 m_roughness;
		alignas(8) const uint32 m_specular;
		alignas(8) const bool m_emissionEnabled;
		alignas(8) const uint32 m_emissionEnergy;
		alignas(8) const uint32 m_occlusionMapValue;
	};

	/**
	 * @brief Construct a new Material object
	 * 
	 * @param albedoColor base color
	 * @param albedoTexturePath base texture path
	 * @param metallic metallic value
	 * @param roughness roughness value
	 * @param metallicTexturePath metallic texture path
	 * @param specularColor specular color
	 * @param specularTexturePath specular texture
	 * @param emissionColor light color if enabled
	 * @param emissionTexturePath light texture path if enabled
	 * @param normalMapTexturePath normal map texture path
	 * @param displacementMapTexturePath height map texture path
	 * @param occlusionColor color of the occluded parts
	 * @param occlusionMapTexturePath occlusion map texture path
	 */
	Material(
		const Color& albedoColor = Color(),
		std::string_view albedoTexturePath = {},
		const float32& metallic = 0.0f,
		const float32& roughness = 0.0f,
		std::string_view metallicTexturePath = {},
		const Color& specularColor = Color(),
		std::string_view specularTexturePath = {},
		const Color& emissionColor = Color(),
		std::string_view emissionTexturePath = {},
		std::string_view normalMapTexturePath = {},
		std::string_view displacementMapTexturePath = {},
		const Color& occlusionColor = Color(),
		std::string_view occlusionMapTexturePath = {}
	);
	/**
	 * @brief constuct a material from material data loaded from a file
	 * 
	 * @param material loaded raw material data
	 */
	Material(const util::detail::LoadedMaterial& material) :
		Material(
			{ material.mats[0].diffuse[0], material.mats[0].diffuse[1], material.mats[0].diffuse[2] },
			material.mats[0].diffuse_texname,
			material.mats[0].metallic,
			material.mats[0].roughness,
			material.mats[0].metallic_texname,
			{ material.mats[0].specular[0], material.mats[0].specular[1], material.mats[0].specular[2] },
			material.mats[0].specular_texname,
			{ material.mats[0].emission[0], material.mats[0].emission[1], material.mats[0].emission[2] },
			material.mats[0].emissive_texname,
			material.mats[0].normal_texname,
			material.mats[0].displacement_texname,
			{ material.mats[0].ambient[0], material.mats[0].ambient[1], material.mats[0].ambient[2] },
			material.mats[0].ambient_texname
		) { path = material.path; }
	
	/**
	 * @brief a function to update the internal buffers after material properties have changed
	 */
	void update_buffers();
	
	Color albedoColor;
	HashedTexture albedoTexture;

	float32 metallic;
	float32 roughness;
	HashedTexture metallicTexture;

	Color specularColor;
	HashedTexture specularTexture;

	Color emissionColor;
	HashedTexture emissionTexture;
	
	HashedTexture normalMapTexture;

	HashedTexture displacementMapTexture;

	Color occlusionColor;
	HashedTexture occlusionMapTexture;

	std::string path;
	
private:
	Array<UniquePointer<vulkan::GPUBuffer>, config::maxFramesInFlight> m_fragShaderBuffers;
	Array<UniquePointer<vulkan::GPUBuffer>, config::maxFramesInFlight> m_vertShaderBuffers;

	/**
	 * @brief System to render the material by binding it to a camera
	 */
	class MaterialSystem {
	public:
		MaterialSystem() = default;
		/**
		 * @brief Construct a new Material object
		 * 
		 * @param camera camera which will draw the material
		 * @param material material to draw
		 */
		MaterialSystem(Camera* const camera, Material* const material);

		/**
		 * @brief get the descriptor sets
		 *
		 * @return const Array<vulkan::DescriptorSystem::DescriptorSet*, config::maxFramesInFlight>&
		*/
		NODISCARD const Array<vulkan::DescriptorSystem::DescriptorSetResource, config::maxFramesInFlight>& descriptorSets() const noexcept { return m_descriptorSets; }

	private:
		Camera* m_camera;
		Material* m_material;

		Array<vulkan::DescriptorSystem::DescriptorSetResource, config::maxFramesInFlight> m_descriptorSets;

		struct MaterialVertexData {
			alignas(8) const uint32 m_normalMapValue;
			alignas(8) const int32 m_displacementMapValue;
		};

		struct MaterialFragmentData {
			alignas(16) const glm::vec4 m_albedoColor;
			alignas(16) const glm::vec4 m_emissionColor;
			alignas(8) const uint32 m_metallic;
			alignas(8) const uint32 m_roughness;
			alignas(8) const uint32 m_specular;
			alignas(8) const bool m_emissionEnabled;
			alignas(8) const uint32 m_emissionEnergy;
			alignas(8) const uint32 m_occlusionMapValue;
		};

		/**
		 * @brief draw all meshes using the material
		 */
		void draw() const;

		friend class Framebuffers;
		friend class Camera;
	};

	friend class Framebuffers;
	friend class Camera;
};

} // namespace lyra
