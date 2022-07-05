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

/** 
const float FPS() {
	auto startTime = std::chrono::high_resolution_clock::now();

	auto currentTime = std::chrono::high_resolution_clock::now();
	return 1.f/std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count()/1000.f;
}

const float deltaTime() {
	return 1/FPS();
}
*/

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

}
