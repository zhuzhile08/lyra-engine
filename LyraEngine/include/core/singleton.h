/*************************
 * @file   singleton.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 * 
 * @brief  a base class for singletons
 * 
 * @date   2022-25-7
 * 
 * @copyright Copyright (c) 2022
*************************/

#pragma once

#include <memory>

namespace lyra {

/**
 * @brief singleton template
 * 
 * @tparam _Ty type
 */
template <class _Ty> class Singleton {
protected:
	Singleton() { }
	Singleton(const Singleton<_Ty>&) = delete;
	Singleton operator=(const Singleton<_Ty>&) = delete;

	static std::unique_ptr<_Ty> _singleton;
public:
	static void init() { 
		_singleton = std::move(std::make_unique<_Ty>());
	}

	/**
	 * @brief get the singleton instance
	 * 
	 * @return _Ty* const
	 */
	[[nodiscard]] static _Ty* const get() noexcept { return &*_singleton; }
};

} // namespace lyra
