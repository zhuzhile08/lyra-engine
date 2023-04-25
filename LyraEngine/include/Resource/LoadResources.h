#pragma once

#include <Lyra/Lyra.h>

#include <fstream>
#include <string>
#include <string_view>
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

/**
 * @brief load a asset file
 *
 * @param binPath path for the binary file
 * 
 * @return lyra::util::AssetFile
 */
NODISCARD AssetFile load_assets(std::string_view binPath);

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
 * @return char* const 
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
