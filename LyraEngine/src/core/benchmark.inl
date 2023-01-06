#include <core/benchmark.h>

namespace lyra {

Benchmark::Benchmark() : m_start(std::chrono::high_resolution_clock::now()) { 
	// this isn't techinally a warning but it should stand out amoung all of the other logs
	log().warning("A benchmarker was created at address: ", get_address(this)); 
}

// destruct and stop the timer
Benchmark::~Benchmark() {
	// calculate end time
	auto end = std::chrono::high_resolution_clock::now();
	// calculate time passed since start in miliseconds
	auto elapsed = std::chrono::time_point_cast<std::chrono::microseconds>(end).time_since_epoch().count() - std::chrono::time_point_cast<std::chrono::microseconds>(m_start).time_since_epoch().count();

	log().warning("The benchmarker at address", get_address(this), " exited its scope with a time of: ", elapsed, " microseconds!");
}

} // namespace lyra