#pragma once

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
const non_access::AssetFile& load_binary(std::string binPath);

/**
 * @brief ldat file format documentation:
 * 
 * 4 bytes, char, type of binary
 * 4 bytes, uint32_t, length of the compressed data
 * the rest is the compressed data
 */

/**
 * @brief decompress the json of a asset file
 * 
 * @param assetFile asset file
 * @param jsonLength length of the compressed json file
 * @param jsonSize size of the original json file
 * 
 * @return char*&
 */
char* &unpack_json(const char* const data, uint32 jsonLength, uint32 jsonSize);

/**
 * @brief lson file format documentation:
 * 
 * 4 bytes, char, type of json
 * 4 bytes, uint32_t, length of the compressed json data
 * 4 bytes, uint32_t, length of the original json file
 * the rest is the compressed json
 */

} // namespace lyra
