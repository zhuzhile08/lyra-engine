#pragma once

#include <core/defines.h>
#include <core/logger.h>

#include <vector>
#include <string>
#include <fstream>

namespace lyra {

std::vector <char> read_binary(std::string path);

std::vector <char> read_text(std::string path);

}
