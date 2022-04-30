#pragma once

#include <core/defines.h>
#include <core/logger.h>

#include <vector>
#include <string>
#include <fstream>

namespace lyra {

std::vector <char> read_binary(string path);

std::vector <char> read_text(string path);

}
