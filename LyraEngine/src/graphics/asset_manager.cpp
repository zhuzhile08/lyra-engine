#include <graphics/asset_manager.h>

namespace lyra {

void AssetManager::init() {
	const auto& data = load_assets("data/images/images.ldat"); // custom data formats, baby!

}

const AssetManager::TextureInfo& AssetManager::unpack_texture(const char* path) {
	const auto& begin = _images.binary.begin() + _images.json[path]["begin"];
	const auto& end = begin + _images.json[path]["data_length"]; // IMPORTANT!!! Remember to add one to the length of the data, or else everything will be screwed up
	std::vector<char> compImageData(begin, end);
	std::vector<char> imageData = { };

	LZ4_decompress_safe(compImageData.data(), imageData.data(), _images.json[path]["data_length"], _images.json[path]["size"]);

	AssetManager::TextureInfo textureInfo{
		
	};



	return textureInfo;
}

} // namespace lyra