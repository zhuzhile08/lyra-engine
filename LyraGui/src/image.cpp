#include <image.h>

bool load_image(std::string path, void* data) {
	// load the image
	/**  
	int width, height, channels;
	stbi_uc* imagePixelData = stbi_load(path.c_str(), &width, &height, &channels, STBI_rgb_alpha);
	if (imagePixelData == nullptr) lyra::Logger::log_error("Failed to load image from path: ", path, "!");
	data = imagePixelData;
	*/

	return true;
}
