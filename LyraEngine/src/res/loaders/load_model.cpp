#include <res/loaders/load_model.h>



namespace lyra {

namespace util {

const LoadedModel load_model(const char* path) {
	LoadedModel load;
	// warning and errors
	std::string warning, error;

	// load the model
	tinyobj::LoadObj(&load.vertices, &load.shapes, &load.materials, &error, path);

	// check if there are errors
	if (!warning.empty()) Logger::log_warning("A problem occurred while loading a model: ", warning);
	if (!error.empty()) Logger::log_error("An error occurred while loading a model: ", error);
	if (warning.empty() && error.empty()) Logger::log_debug(Logger::tab(), "Successfully loaded model data from adress ", path, "!", Logger::end_l());

	return load;
}

} // namespace util

} // namespace lyra
