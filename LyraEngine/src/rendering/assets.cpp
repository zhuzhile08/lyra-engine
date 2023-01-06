#include <rendering/assets.h>

#include <utility>

#include <rendering/texture.h>

#include <stb_image.h>

namespace lyra {

util::ImageData Assets::unpack_texture(const char* path) {
	/**  
	const auto& begin = m_images.binary.begin() + m_images.json.at(path).at("begin");
	const auto& end = begin + m_images.json.at(path).at("data_length"); // IMPORTANT!!! Remember to add one to the length of the data, or else everything will be screwed up
	std::vector<char> compImageData(begin, end);
	std::vector<char> imageData = { };

	LZ4_decompress_safe(compImageData.data(), imageData.data(), m_images.json.at(path).at("data_length"), m_images.json.at(path).at("size"));

	nlohmann::json jsonImageData = nlohmann::json::parse(imageData);

	*/

	int width, height, channels;
	stbi_uc* imagePixelData = stbi_load(path, &width, &height, &channels, STBI_rgb_alpha);
	if (imagePixelData == nullptr) log().exception("Failed to load image from path: ", path, "!");

	util::ImageData imageData{
		path,
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

Texture* Assets::operator[](const char* path) {
	if (!m_textures.contains(path)) {
		m_textures.emplace(std::make_pair(path, Texture(unpack_texture(path))));
	}
	return &m_textures.at(path);
}

const Texture* const Assets::operator[](const char* path) const {
	if (!m_textures.contains(path)) {
		m_textures.emplace(std::make_pair(path, Texture(unpack_texture(path))));
	}
	return &m_textures.at(path);
}

std::unordered_map<const char*, Texture> Assets::m_textures;
Texture Assets::m_nullTexture(unpack_texture("data/img/Default.bmp"));
Texture Assets::m_nullNormal(unpack_texture("data/img/Normal.bmp"));

} // namespace lyra
