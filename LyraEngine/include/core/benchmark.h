/*************************
 * @file benchmark.h
 * @author zhuzhile08 (zhuzhile08@gmail.com)
 * 
 * @brief A scope based benchmarking tool
 * 
 * @date 2022-10-11
 * 
 * @copyright Copyright (c) 2022
 *************************/

#pragma once

#include <chrono>

namespace lyra {

/**
 * @brief small scope-based benchmarking class
 */
class Benchmark {
public:
	// initialize and start the timer
	Benchmark();
	
	// destruct and stop the timer
	~Benchmark();

private:
	std::chrono::time_point<std::chrono::high_resolution_clock> m_start;
};

} // namespace lyra
