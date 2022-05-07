#include <res/loaders/load_model.h>

namespace lyra {

non_access::LoadedModel load_model(const std::string path) {
	non_access::LoadedModel load;
	// warning and errors
	std::string warning, error;

	// load the model
	tinyobj::LoadObj(&load.vertices, &load.shapes, &load.materials, &warning, &error, path.c_str(), nullptr);

	// check if there are errors
	if (!warning.empty()) Logger::log_warning("A problem occured while loading a model: ", warning);
	if (!error.empty()) Logger::log_error("An error occured while loading a model: ", error);
	if (warning.empty() && error.empty()) Logger::log_debug(Logger::tab(), "Succesfully loaded model data from adress ", path, "!", Logger::end_l());

	return load;
}

} // namespace lyra
