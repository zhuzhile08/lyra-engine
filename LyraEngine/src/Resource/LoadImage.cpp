#include <Resource/LoadImage.h>

#include <Common/Logger.h>

#include <stb_image.h>

namespace lyra {

namespace util {

detail::LoadedImage load_image(std::string_view path) {
	/**  
	const auto& begin = m_images.binary.begin() + m_images.json.at(path).at("begin");
	const auto& end = begin + m_images.json.at(path).at("data_length"); // IMPORTANT!!! Remember to add one to the length of the data, or else everything will be screwed up
	std::vector<char> compImageData(begin, end);
	std::vector<char> imageData = { };

	LZ4_decompress_safe(compImageData.data(), imageData.data(), m_images.json.at(path).at("data_length"), m_images.json.at(path).at("size"));

	nlohmann::json jsonImageData = nlohmann::json::parse(imageData);

	*/

	int width, height, channels;
	stbi_uc* imagePixelData = stbi_load(path.data(), &width, &height, &channels, STBI_rgb_alpha);
	if (imagePixelData == nullptr) ASSERT(false, "Failed to load image from path: {}!", path);

	util::detail::LoadedImage imageData{
		(uint32) width, // jsonImageData.at("width"),
		(uint32) height, // jsonImageData.at("height"),
		0, // jsonImageData.at("length"),
		1, // jsonImageData.at("mipmap"),
		0, // jsonImageData.at("type"),
		1, // jsonImageData.at("alpha"),
		1, // jsonImageData.at("dimension"),
		1, // jsonImageData.at("wrap"),
		1, // jsonImageData.at("anistropy"),
		imagePixelData // (void*)&jsonImageData.at("data")
	};

	return imageData;
}

} // namespace util

} // namespace lyra
