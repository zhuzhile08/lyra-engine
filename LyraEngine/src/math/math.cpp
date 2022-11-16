#include <math/math.h>

namespace lyra {

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

void decompose_transform_matrix(const glm::mat4& matrix, glm::vec3& translation, glm::vec3& rotation, glm::vec3& scale) {
	// the reason this function exists if because the glm matrix decompose just looks... very big
	// and it doesn't work with euler angles
	auto mat(matrix);
	// get the translation
	translation = glm::vec3(matrix[3]);
	mat[3] = glm::vec4{};
	// get the scale
	scale = glm::vec3(length(matrix[0]), length(matrix[1]), length(matrix[2]));
	mat[0] /= scale.x;
	mat[1] /= scale.y;
	mat[2] /= scale.z;
	// get the rotation in euler angles
	rotation.y = glm::degrees(asin(-mat[0][2])); 	
	if (cos(rotation.y) != 0) {
		rotation.x = glm::degrees(atan2(mat[1][2], mat[2][2]));
		rotation.z = glm::degrees(atan2(mat[0][1], mat[0][0]));
	} else {
		rotation.x = glm::degrees(atan2(-mat[2][0], mat[1][1]));
		rotation.z = 0;
	}
}

constexpr glm::mat4 perspective_matrix(const float& fov, const float& width, const float& height, const float& near, const float& far) {
	float tanHalfFov = tan(glm::radians(fov) / 2);

	return glm::mat4 {
		1.0f / ((width / height) * tanHalfFov),	0.0f, 				0.0f, 							0.0f,
		0.0f, 									-1.0f / tanHalfFov, 	0.0f, 							0.0f, 
		0.0f, 									0.0f, 				-(far + near) / (far - near), 			-(2 * near * far) / (far - near),
		0.0f, 									0.0f, 				1.0f, 	0.0f
	};
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
