#include <graphics/asset_manager.h>

namespace lyra {

void AssetManager::init() {
	_images = load_assets("data/images/images.ldat"); // custom data formats, baby!

}

const AssetManager::TextureInfo AssetManager::unpack_texture(const char* path) {
	/**  
	const auto& begin = _images.binary.begin() + _images.json.at(path).at("begin");
	const auto& end = begin + _images.json.at(path).at("data_length"); // IMPORTANT!!! Remember to add one to the length of the data, or else everything will be screwed up
	std::vector<char> compImageData(begin, end);
	std::vector<char> imageData = { };

	LZ4_decompress_safe(compImageData.data(), imageData.data(), _images.json.at(path).at("data_length"), _images.json.at(path).at("size"));

	nlohmann::json jsonTextureInfo = nlohmann::json::parse(imageData);

	*/

	int width, height, channels;
	stbi_uc* imagePixelData = stbi_load(path, &width, &height, &channels, STBI_rgb_alpha);
	if (imagePixelData == nullptr) lyra::Logger::log_error("Failed to load image from path: ", path, "!");
	void* data = imagePixelData;

	AssetManager::TextureInfo textureInfo{
		width, // jsonTextureInfo.at("width"),
		height, // jsonTextureInfo.at("height"),
		0, // jsonTextureInfo.at("length"),
		3, // jsonTextureInfo.at("mipmap"),
		0, // jsonTextureInfo.at("type"),
		1, // jsonTextureInfo.at("alpha"),
		1, // jsonTextureInfo.at("dimension"),
		1, // jsonTextureInfo.at("wrap"),
		1, // jsonTextureInfo.at("anistropy"),
		data // (void*)&jsonTextureInfo.at("data")
	};

	return textureInfo;
}

non_access::AssetFile AssetManager::_images;

} // namespace lyra
