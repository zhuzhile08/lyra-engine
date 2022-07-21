#include <math/math.h>

namespace lyra {

float pyth(const glm::vec2 a, const glm::vec2 b) {
	float x = b.x - a.x, y = b.y - a.y;
	return sqrt(x * x + y * y);
}

float pyth3(const glm::vec3 a, const glm::vec3 b) {
	float x = b.x - a.x, y = b.y - a.y, z = b.z - a.z;
	return sqrt(x * x + y * y + z * z);
}

template <class Ty> Ty point_on_line(Ty first, Ty second, float value) {
	return first + (second - first) * value;
}

template<class Ty> Ty bezier(std::vector<Ty> points, float value) {
	std::vector<Ty> remaining_points;

	for (int i = 0; i <= points.size(); i++) remaining_points.push_back(point_on_line<Ty>(points.at(i), points.at(i + 1), value));
	
	if (remaining_points.size() == 2) return point_on_line<Ty>(points.at(0), points.at(1), value);

	bezier<Ty>(remaining_points, value);
}

float randDoub(const float x, const float y) {
	int precision = rand() % 1000000 + 100;			// calculate the precision
	/**
	 * if roughly works like this:
	 * first you generate a random number unter the precision limit
	 * then it gets multiplied by the difference of the upper and lower limit divided by the precision so that it will always be lower than the difference
	 * at last it will be added to the lower limit because everything else is calculated without it in mind
	 */
	return x + float(rand() % precision) * (y - x)/precision;
}

void alignPointer(void* address, const uint8_t alignment, const uint8_t mode) {
	address = (mode == 1) ? (void*)((reinterpret_cast<uintptr_t>(address) + static_cast<uintptr_t>(alignment - 1)) & static_cast<uintptr_t>(~(alignment - 1))) :
		(void*)(reinterpret_cast<uintptr_t>(address) & static_cast<uintptr_t>(~(alignment - 1)));
}

const uint8_t alignPointerAdjustment(const void* address, const uint8_t alignment, const uint8_t mode) {
	uint8_t adjustment = (mode == 1) ? alignment - (reinterpret_cast<uintptr_t>(address) & static_cast<uintptr_t>(alignment - 1)) : 
		(reinterpret_cast<uintptr_t>(address) & static_cast<uintptr_t>(alignment - 1));

	return adjustment;
}

}
