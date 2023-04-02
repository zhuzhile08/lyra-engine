#include <Resource/LoadModel.h>

#include <Common/Logger.h>

namespace lyra {

namespace util {

const LoadedModel load_model(std::string_view path) {
	LoadedModel load;
	// warning and errors
	std::string error, warning;

	// load the model
	tinyobj::LoadObj(&load.vertices, &load.shapes, &load.materials, &warning, &error, path.data());

	// check if there are warnings
	if (!warning.empty()) log().warning("A problem occurred while loading a model: ", warning);
	// check if there are errors
	if (!error.empty()) log().error("An error occurred while loading a model: ", error);

	return load;
}

} // namespace util

} // namespace lyra
