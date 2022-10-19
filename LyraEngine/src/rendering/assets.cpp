#include <rendering/assets.h>



#include <rendering/texture.h>

#include <stb_image.h>

namespace lyra {

const Assets::TextureInfo Assets::unpack_texture(const char* path) {
	/**  
	const auto& begin = m_images.binary.begin() + m_images.json.at(path).at("begin");
	const auto& end = begin + m_images.json.at(path).at("data_length"); // IMPORTANT!!! Remember to add one to the length of the data, or else everything will be screwed up
	std::vector<char> compImageData(begin, end);
	std::vector<char> imageData = { };

	LZ4_decompress_safe(compImageData.data(), imageData.data(), m_images.json.at(path).at("data_length"), m_images.json.at(path).at("size"));

	nlohmann::json jsonTextureInfo = nlohmann::json::parse(imageData);

	*/

	int width, height, channels;
	stbi_uc* imagePixelData = stbi_load(path, &width, &height, &channels, STBI_rgb_alpha);
	if (imagePixelData == nullptr) Logger::log_exception("Failed to load image from path: ", path, "!");

	Assets::TextureInfo textureInfo{
		(uint32) width, // jsonTextureInfo.at("width"),
		(uint32) height, // jsonTextureInfo.at("height"),
		0, // jsonTextureInfo.at("length"),
		1, // jsonTextureInfo.at("mipmap"),
		0, // jsonTextureInfo.at("type"),
		1, // jsonTextureInfo.at("alpha"),
		1, // jsonTextureInfo.at("dimension"),
		1, // jsonTextureInfo.at("wrap"),
		1, // jsonTextureInfo.at("anistropy"),
		imagePixelData // (void*)&jsonTextureInfo.at("data")
	};

	return textureInfo;
}

util::AssetFile Assets::m_images; // = util::load_assets("data/images/images.ldat");
SmartPointer<Texture> Assets::m_nullTexture = SmartPointer<Texture>::create("data/img/Default.bmp");
SmartPointer<Texture> Assets::m_nullNormal = SmartPointer<Texture>::create("data/img/Normal.bmp");

} // namespace lyra
