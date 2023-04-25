#include <Resource/LoadMaterial.h>

#include <Common/Logger.h>
#include <map>

#include <Resource/LoadFile.h>

namespace lyra {

namespace util {

LoadedMaterial load_material(std::string_view path) {
	// TOL material data
	LoadedMaterial material;
	material.path = path;

	// warning and errors
	std::string error, warning;

	// basically useless
	std::map<std::string, int> mats;

	// load the material
	std::ifstream file; // raw file
	load_file(path, OpenMode::MODE_INPUT, file);
	tinyobj::LoadMtl(&mats, &material.mats, &file, &warning, &error);

	// check if there are warnings
	if (!warning.empty()) log().warning("A problem occurred while loading a material: ", warning);
	// check if there are errors
	if (!error.empty()) log().error("An error occurred while loading a material: ", error);

	return material;
}

} // namespace util

} // namespace lyra
