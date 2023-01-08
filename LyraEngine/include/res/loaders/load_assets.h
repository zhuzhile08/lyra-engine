#pragma once

#include <lyra.h>

#include <fstream>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>
#include <lz4.h>

namespace lyra {

namespace util {

// raw asset file
struct AssetFile {
	char type[4];
	nlohmann::json json;
	std::vector<char> binary;
};

// decompressed texture information
struct ImageData {
	// texture path
	const char* path;
	// texture width
	uint32 width;
	// texture height
	uint32 height;
	// texture length, exclusive to 3D images/textures
	uint32 length;
	// mipmapping levels
	uint32 mipmap;
	// type of texture
	uint32 type;
	// how to treat the alpha value of the image
	uint32 alpha;
	// how the UVs should read the image
	uint32 dimension;
	// how to wrap the image if the UVs exceeds the border of the image
	uint32 wrap;
	// anistropic filtering
	uint32 anistropy;
	// image data
	void* data;
}; // this also roughly represents the texture data file

// decompressed mesh information
struct MeshInfo {

};

/**
 * @brief load a asset file
 *
 * @param binPath path for the binary file
 * 
 * @return const lyra::util::AssetFile
 */
NODISCARD const AssetFile load_assets(const std::string& binPath);

/**
 * @brief ldat file format documentation:
 * 
 * 4 bytes, char, type of binary
 * 4 bytes, uint32_t, length of the compressed data
 * the rest is the compressed data
 */

#ifdef _WIN32

/**
 * @brief decompress a file
 * 
 * @param data data to decompress
 * @param length length of the compressed file
 * @param size size of the original file
 * 
 * @return char* const 
 */
NODISCARD char* const unpack_file(const char* data, const uint32& length, const uint32& size);

#elif __APPLE__

/**
 * @brief decompress a file
 * 
 * @param data data to decompress
 * @param length length of the compressed file
 * @param size size of the original file
 * 
 * @return constexpr char* const 
 */
NODISCARD constexpr char* const unpack_file(const char* data, const uint32& length, const uint32& size);

constexpr char* const unpack_file(const char* data, const uint32& jsonLength, const uint32& jsonSize) {
	char* result = { };
	LZ4_decompress_safe(data, result, jsonLength, jsonSize);
	return result;
}

#endif

/**
 * @brief lson file format documentation:
 * 
 * 4 bytes, char, type of json
 * 4 bytes, uint32_t, length of the compressed json data
 * 4 bytes, uint32_t, length of the original json file
 * the rest is the compressed json
 */

} // util

} // namespace lyra
