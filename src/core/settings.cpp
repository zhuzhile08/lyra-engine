#include <core/settings.h>

namespace lyra {

const std::vector <const char*> Settings::Debug::requestedDeviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
const std::vector <const char*> Settings::Debug::requestedValidationLayers = { "VK_LAYER_KHRONOS_validation" };

} // namespace lyra
