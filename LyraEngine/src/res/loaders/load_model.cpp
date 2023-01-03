#include <res/loaders/load_model.h>

namespace lyra {

namespace util {

const LoadedModel load_model(const char* path) {
	LoadedModel load;
	// warning and errors
	std::string error, warning;

	// load the model
	tinyobj::LoadObj(&load.vertices, &load.shapes, &load.materials, &warning, &error, path);

	// check if there are warnings
	if (!warning.empty()) Logger::log_warning("A problem occurred while loading a model: ", warning);
	// check if there are errors
	if (!error.empty()) Logger::log_error("An error occurred while loading a model: ", error);

	return load;
}

} // namespace util

} // namespace lyra
