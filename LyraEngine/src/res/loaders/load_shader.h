#pragma once

#include <res/loaders/load_file.h>

#include <core/logger.h>

#include <vector>
#include <fstream>

namespace lyra {

/**
 * @brief read a shader form a .spv binary file
 * 
 * @param path path
 * @param data empty data to load into
 */
void read_shader_binary(const char* const path, std::vector <char>& data);

}
