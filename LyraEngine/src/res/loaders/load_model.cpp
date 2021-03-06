#include <res/loaders/load_model.h>

namespace lyra {

const non_access::LoadedModel load_model(const char* path) {
	non_access::LoadedModel load;
	// warning and errors
	std::string warning, error;

	// load the model
	tinyobj::LoadObj(&load.vertices, &load.shapes, &load.materials, &warning, &error, path, nullptr);

	// check if there are errors
	if (!warning.empty()) Logger::log_warning("A problem occurred while loading a model: ", warning);
	if (!error.empty()) Logger::log_error("An error occurred while loading a model: ", error);
	if (warning.empty() && error.empty()) Logger::log_debug(Logger::tab(), "Successfully loaded model data from adress ", path, "!", Logger::end_l());

	return load;
}

} // namespace lyra
