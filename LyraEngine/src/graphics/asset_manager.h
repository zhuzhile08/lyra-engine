/*************************
 * @file lyra.h
 *
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 *
 * @brief wrapper the basic AssetManager features
 *
 * @date 2022-06-19
 *
 * @copyright Copyright (c) 2022
 *************************/

#pragma once

#include <core/logger.h>
#include <res/loaders/load_binary.h>

#include <fstream>
#include <string>
#include <vector>

namespace lyra {

class AssetManager {
	/**
	 * @brief destructor of the window
	**/
	~AssetManager() noexcept { }

	AssetManager() noexcept = delete;
	AssetManager(const AssetManager&) noexcept = delete;
	AssetManager operator=(const AssetManager&) const noexcept = delete;
	/**
	 * @brief initialize the asset manager and load all the raw data
	 */
	static void init();

	/**
	 * @brief return the raw image data
	 * 
	 * @return const lyra::non_access::AssetFile&
	 */
	[[nodiscard]] static const non_access::AssetFile& images() noexcept { return _images; }

private:
	static non_access::AssetFile _images;
	
};

} // namespace lyra