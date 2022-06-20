/*************************
 * @file lyra.h
 *
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 *
 * @brief wrapper the basic application features
 *
 * @date 2022-06-19
 *
 * @copyright Copyright (c) 2022
 *************************/

#pragma once

#include <core/logger.h>

#include <fstream>
#include <string>
#include <vector>

namespace lyra {

class AssetManager {
private:
	struct AssetFile {
		char type[4];
		int version;
		std::string json;
		std::vector<char> binary;

		/**
		 * @brief load the asset file
		 * 
		 * @param path path
		*/
		void load(std::string path);
		/**
		 * @brief store new data into the asset file
		 *
		 * @param path path
		*/
		void store(std::string path);
	};
};

} // namespace lyra