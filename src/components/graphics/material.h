#pragma once

#include <graphics/asset_mamager.h>
#include <core/rendering/vulkan/descriptor.h>
#include <lyra.h>

namespace lyra {

class Material {
private:
	uint32 textureID, pipelineID;
	VulkanDescriptor descriptor;
};

} // namespace lyra