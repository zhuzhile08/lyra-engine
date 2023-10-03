#include <Resource/LoadMaterial.h>

#include <Common/Logger.h>
#include <map>

#include <Common/FileSystem.h>

namespace lyra {

namespace resource {

MaterialFile loadMaterialFile(const std::filesystem::path& path) {
	// TOL material data
	MaterialFile material;
	material.path = path;

	// warning and errors
	std::string error, warning;

	// basically useless
	std::map<std::string, int> mats;

	// load the material
	std::ifstream file; // raw file
	//filesystem::load_file(path, filesystem::OpenMode::input, file);
	tinyobj::LoadMtl(&mats, &material.mats, &file, &warning, &error);

	// check if there are warnings
	if (!warning.empty()) log::warning("A problem occurred while loading a material: {}!", warning);
	// check if there are errors
	if (!error.empty()) log::error("An error occurred while loading a material: {}!", error);

	return material;
}

} // namespace resource

} // namespace lyra
