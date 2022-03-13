#include <math/math.h>

namespace lyra {

double pyth(glm::vec2 a, glm::vec2 b) {
	double x = b.x - a.x, y = b.y - a.y, result;
	result = sqrt(x * x + y * y);
	return result;
}

double pyth3(glm::vec3 a, glm::vec3 b) {
	double x = b.x - a.x, y = b.y - a.y, z = b.z - a.z, result;
	result = sqrt(x * x + y * y + z * z);
	return result;
}

double deltaTime(double FPS) {
	return 1/FPS;
}

double randDoub(double x, double y) {
	int precision = rand() % 1000000 + 100;			// calculate the precision
	/**
	 * if roughly works like this:
	 * first you generate a random number unter the precision limit
	 * then it gets multiplied by the difference of the upper and lower limit divided by the precision so that it will always be lower than the difference
	 * at last it will be added to the lower limit because everything else is calculated without it in mind
	 */
	return x + double(rand() % precision) * (y - x)/precision;
}

}
