#include <res/loaders/load_assets.h>

namespace lyra {

const non_access::AssetFile load_assets(const std::string binPath) {
	non_access::AssetFile loadedAsset;

	// check
	if (binPath.substr(binPath.length() - 4, 4) != "ldat") Logger::log_warning("Non standard file extension found on data file at path: ", binPath, "! This may cause problems during loading.");

	// load the binary
	std::ifstream binInFile; load_file(binPath.c_str(), OpenMode::MODE_BINARY, binInFile);

	// read the type of the asset
	binInFile.read(loadedAsset.type, 4);

	// length of the binary file
	uint32 binLength = 0;
	binInFile.read((char*)&binLength, sizeof(uint32));
	// resize and read the binary file
	loadedAsset.binary.resize(binLength);
	binInFile.read(loadedAsset.binary.data(), binLength);

	// create the path for the compressed json
	std::string jsonPath = binPath;
	jsonPath.replace(binPath.length() - 4, 4, "lson");

	// load the json
	std::ifstream jsonInFile; load_file(jsonPath.c_str(), OpenMode::MODE_BINARY, jsonInFile);

	// read the type of the json
	char type[4] = { };
	jsonInFile.read(type, sizeof(char[4]));
	if (type != loadedAsset.type) Logger::log_exception("Loaded compressed JSON file at: ", jsonPath, " is not the same type as the corresponding asset file loaded at: ", binPath, "!"); // check if binary and json are for the same type of stuff

	// length of the compressed json
	uint32 jsonLength = 0;
	jsonInFile.read((char*)&jsonLength, sizeof(uint32));

	// length of the decompressed json
	uint32 jsonDecompLength = 0;
	jsonInFile.read((char*)&jsonDecompLength, sizeof(uint32));

	// read the compressed json
	char* json = { };
	jsonInFile.read(json, jsonLength);
	// decompress the json
	json = unpack_file(json, jsonLength, jsonDecompLength);
	loadedAsset.json = nlohmann::json::parse(json);

	// close the files
	jsonInFile.close();
	binInFile.close();

	return loadedAsset;
}

char* const unpack_file(const char* const data, const uint32 jsonLength, const uint32 jsonSize) {
	char* result = { };
	LZ4_decompress_safe(data, result, jsonLength, jsonSize);
	return result;
}

} // namespace lyra
