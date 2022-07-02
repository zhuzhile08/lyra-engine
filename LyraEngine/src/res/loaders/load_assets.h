#pragma once

#include <res/loaders/load_file.h>

#include <core/logger.h>

#include <fstream>
#include <string>
#include <vector>
#include <json.hpp>
#include <lz4.h>

namespace lyra {

namespace non_access {

struct AssetFile {
	char type[4];
	nlohmann::json json;
	std::vector<char> binary;
};

} // non_access

/**
 * @brief load a asset file
 *
 * @param binPath path for the binary file
 * 
 * @return const lyra::non_access::AssetFile
 */
[[nodiscard]] const non_access::AssetFile load_assets(const std::string binPath);

/**
 * @brief ldat file format documentation:
 * 
 * 4 bytes, char, type of binary
 * 4 bytes, uint32_t, length of the compressed data
 * the rest is the compressed data
 */

/**
 * @brief decompress a file
 * 
 * @param data data to decompress
 * @param length length of the compressed file
 * @param size size of the original file
 * 
 * @return char* const 
 */
[[nodiscard]] char* const unpack_file(const char* const data, const uint32 length, const uint32 size);

/**
 * @brief lson file format documentation:
 * 
 * 4 bytes, char, type of json
 * 4 bytes, uint32_t, length of the compressed json data
 * 4 bytes, uint32_t, length of the original json file
 * the rest is the compressed json
 */

} // namespace lyra
