#include <res/loaders/load_model.h>

namespace lyra {

non_access::LoadedModel load_model(const str path) {
	non_access::LoadedModel load;
	// warning and errors
	std::string warning, error;

	// load the model
	tinyobj::LoadObj(&load.vertices, &load.shapes, &load.materials, &warning, &error, path, nullptr);

	// check if there are errors
	if (!warning.empty()) LOG_WARNING("A problem occured while loading a model: ", warning);
	if (!error.empty()) LOG_ERROR("An error occured while loading a model: ", error);
	if (warning.empty() && error.empty()) LOG_DEBUG(TAB, "Succesfully loaded model data from adress ", path, "!", END_L);

	return load;
}

} // namespace lyra
