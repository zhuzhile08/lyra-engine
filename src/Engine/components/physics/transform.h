#pragma once

#include <glm.hpp>
#include <math/math.h>
#include <math.h>

namespace lyra {

class TransformComponent {
private:
	glm::vec3 position = { 0, 0, 0 }, rotation = { 0, 0, 0 }, scale = { 1, 1, 1 };

protected:
	void translate(glm::vec3 velocity);
	void rotate(glm::vec3 rotation);
	void translate(glm::vec3 velocity, glm::vec3 point);
	void rotate(glm::vec3 rotation, glm::vec3 point);

	glm::vec3 get_local_position();
	glm::vec3 get_local_rotation();
	glm::vec3 get_local_scale();

	void set_position(glm::vec3 newPosition);
	void set_rotation(glm::vec3 newRotation);
	void set_rotation(glm::vec3 rotation, glm::vec3 point);
	void set_scale(glm::vec3 newScale);
};

}
