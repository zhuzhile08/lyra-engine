#include <res/loaders/load_model.h>

namespace lyra {

namespace util {

const LoadedModel load_model(const char* path) {
	LoadedModel load;
	// warning and errors
	std::string error;

	// load the model
#ifdef __APPLE__ // so apparently these functions are different depending on the platform...
	tinyobj::LoadObj(&load.vertices, &load.shapes, &load.materials, &error, path);
#elif _WIN32
	std::string warning;
	tinyobj::LoadObj(&load.vertices, &load.shapes, &load.materials, &warning, &error, path);

	// check if there are warnings
	if (!warning.empty()) Logger::log_warning("A problem occurred while loading a model: ", warning);
#endif

	// check if there are errors
	if (!error.empty()) Logger::log_error("An error occurred while loading a model: ", error);

	return load;
}

} // namespace util

} // namespace lyra
