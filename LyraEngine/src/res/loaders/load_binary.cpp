#include <res/loaders/load_binary.h>

namespace lyra {

const non_access::AssetFile& load_binary(std::string binPath) {
	non_access::AssetFile loadedAsset;

	// check
	if (binPath.substr(binPath.length() - 4, 4) != "ldat") Logger::log_warning("Non standard file extension found on data file at path: ", binPath, "! This may cause problems during loading.");

	// load the binary
	std::ifstream binInFile;
	binInFile.open(binPath, std::ios::binary);
	if (!binInFile.is_open()) Logger::log_exception("Failed to open an asset file for reading at path: ", binPath, "!");
	Logger::log_debug(Logger::tab(), "Successfully loaded asset file in at path ", binPath, "!");

	// read the file contentents
	binInFile.seekg(0);

	// read the type of the asset
	binInFile.read(loadedAsset.type, 4);

	// length of the binary file
	uint32_t binLength = 0;
	binInFile.read((char*)&binLength, sizeof(uint32));
	// resize and read the binary file
	loadedAsset.binary.resize(binLength);
	binInFile.read(loadedAsset.binary.data(), binLength);

	// create the path for the compressed json
	std::string jsonPath = binPath;
	jsonPath.replace(binPath.length() - 4, 4, "lson");

	// load the json
	std::ifstream jsonInFile;
	binInFile.open(jsonPath, std::ios::binary);
	if (!jsonInFile.is_open()) Logger::log_exception("Failed to open an asset file for reading at path: ", binPath, "!");
	Logger::log_debug(Logger::tab(), "Successfully loaded asset file in at path ", binPath, "!");

	// read the file contentents
	binInFile.seekg(0);

	// read the type of the json
	char type[4] = { };
	jsonInFile.read(type, sizeof(char[4]));
	if (type != loadedAsset.type) Logger::log_exception("Loaded compressed JSON file at: ", jsonPath, " is not the same type as the corresponding asset file loaded at: ", binPath, "!"); // check if binary and json are for the same type of stuff

	// length of the compressed json
	uint32_t jsonLength = 0;
	jsonInFile.read((char*)&jsonLength, sizeof(uint32));

	// length of the decompressed json
	uint32_t jsonDecompLength = 0;
	jsonInFile.read((char*)&jsonDecompLength, sizeof(uint32));

	// read the compressed json
	char* json;
	jsonInFile.read(json, jsonLength);
	// decompress the json
	json = unpack_json(json, jsonLength, jsonDecompLength);
	loadedAsset.json = nlohmann::json::parse(json);

	return loadedAsset;
}

char*& unpack_json(const char* const data, uint32 jsonLength, uint32 jsonSize) {
	char* result;
	LZ4_decompress_safe(data, result, jsonLength, jsonSize);
	return result;
}

} // namespace lyra
