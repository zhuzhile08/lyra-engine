#include <res/loaders/load_binary.h>

namespace lyra {

const non_access::AssetFile& load_binary(std::string binPath) {
	non_access::AssetFile loadedAsset;

	// check
	if (binPath.substr(binPath.length() - 4, 4) != "ldat") Logger::log_warning("Non standard file extension found on data file at path: ", binPath, "! This may cause problems during loading.");
	// create the path for the compressed json
	std::string jsonPath = binPath;
	jsonPath.replace(binPath.length() - 4, 4, "lson");

	// load the binary
	std::ifstream binInFile;
	binInFile.open(binPath, std::ios::binary);
	if (!binInFile.is_open()) Logger::log_exception("Failed to open an asset file for reading at path: ", binPath, "!");
	Logger::log_debug(Logger::tab(), "Successfully loaded asset file in at path ", binPath, "!");

	// read the file contentents
	binInFile.seekg(0);

	binInFile.read(loadedAsset.type, 4);

	uint32_t binLength = 0;
	binInFile.read((char*)&binLength, sizeof(uint32_t));
	loadedAsset.binary.resize(binLength);
	binInFile.read(loadedAsset.binary.data(), binLength);

	// load the json
	std::ifstream jsonInFile;
	binInFile.open(jsonPath, std::ios::binary);
	if (!jsonInFile.is_open()) Logger::log_exception("Failed to open an asset file for reading at path: ", binPath, "!");
	Logger::log_debug(Logger::tab(), "Successfully loaded asset file in at path ", binPath, "!");

	uint32_t jsonLength = 0;
	jsonInFile.read((char*)&jsonLength, sizeof(uint32_t));
	loadedAsset.json.resize(jsonLength);
	jsonInFile.read(loadedAsset.json.data(), jsonLength);

	return loadedAsset;
}

} // namespace lyra
