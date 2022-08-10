/*************************
 * @file material.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 *
 * @brief a material system
 *
 * @date 2022-05-27
 *
 * @copyright Copyright (c) 2022
 *************************/

#pragma once

#include <core/decl.h>
#include <nodes/node.h>

#include <memory>

namespace lyra {

class Material {
public:
	Material() { }

	/**
	 * @brief Construct a new Material object
	 * 
	 * @param renderer renderer the material is drawn uses
	 * @param meshRenderers mesh renderers with the material
	 * @param albedoColor color for the default
	 * @param albedoTexture texture for the default
	 * @param metallic metallic value
	 * @param roughness roughness value
	 * @param specular specular value
	 * @param metallicTexture metallic texture
	 * @param emissionEnabled texture light emission
	 * @param emissionColor light color if enabled
	 * @param emissionTexture light texture if enabled
	 * @param emissionEnergy light energy value if enabled
	 * @param normalMapTexture normal map texture
	 * @param normalMapValue normal map strength
	 * @param heightMap height map texture
	 * @param heightMapValue height map strength
	 * @param occlusionMap occlusion map texture
	 * @param occlusionMapValue occlusion map value
	 */
	Material(
		const Camera* const camera,
		const std::vector<MeshRenderer*> meshRenderers,
		const Color albedoColor = Color(),
		const Texture* const albedoTexture = nullptr,
		const uint8 metallic = 0,
		const uint8 roughness = 0,
		const uint8 specular = 0,
		const Texture* const metallicTexture = nullptr,
		const bool emissionEnabled = false,
		const Color emissionColor = Color(),
		const Texture* const emissionTexture = nullptr,
		const uint8 emissionEnergy = 0,
		const Texture* const normalMapTexture = nullptr,
		const int8 normalMapValue = 0,
		const Texture* const heightMap = nullptr,
		const uint8 heightMapValue = 0,
		const Texture* const  occlusionMap = nullptr,
		const uint8 occlusionMapValue = 0
	);

	/**
	 * @brief get the descriptor set
	 *
	 * @return const std::shared_ptr<const VulkanDescriptor>
	*/
	[[nodiscard]] const std::shared_ptr<const VulkanDescriptor> descriptor() const noexcept { return _descriptor; }

private:
	Color _albedoColor;
	const Texture* _albedoTexture;

	uint8 _metallic;
	uint8 _roughness;
	uint8 _specular;
	const Texture* _metallicTexture;

	bool _emissionEnabled;
	Color _emissionColor;
	const Texture* _emissionTexture;
	uint8 _emissionEnergy;
	
	const Texture* _normalMapTexture;
	int8 _normalMapValue;

	const Texture* _heightMap;
	uint8 _heightMapValue;

	const Texture* _occlusionMap;
	uint8 _occlusionMapValue;

	std::vector<MeshRenderer*> _meshRenderers;
	std::shared_ptr<VulkanDescriptor> _descriptor;

	const Camera* camera;

	/**
	 * @brief draw all meshes using the material
	 */
	void draw() const;

	friend class Renderer;
	friend class Camera;
};

} // namespace lyra