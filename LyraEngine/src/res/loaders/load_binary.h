#pragma once

#include <core/logger.h>

#include <fstream>
#include <string>
#include <vector>

namespace lyra {

namespace non_access {

struct AssetFile {
	char type[4];
	std::string json;
	std::vector<char> binary;
};

} // non_access

/**
 * @brief load a asset file
 *
 * @param binPath path for the binary file
 * @return const lyra::non_access::AssetFile
 */
const non_access::AssetFile& load_binary(std::string binPath);

/**
 * @brief decompress the json of a asset file
 * 
 * @param assetFile asset file
 * @return const lyra::non_access:AssetFile&
 */
const non_access::AssetFile& unpack_json(non_access::AssetFile& assetFile);

} // namespace lyra
