#include <nodes/physics/colliders.h>



namespace lyra {

BoxCollider::BoxCollider(
		Rigidbody* parent,
		const char* name,
		const glm::vec3 dimensions,
		const bool active,
		const uint32 tag,
		const glm::vec3 position,
		const glm::vec3 rotation,
		const glm::vec3 scale,
		const RotationOrder rotationOrder
	) : Collider(parent, name, tag, active, position, rotation, scale, rotationOrder), 
		_dimensions(dimensions) { 
#ifndef NDEBUG
		log().warning("The parent Rigidbody of the BoxCollider at address: ", get_address(this), " has been specified as a nullptr. Please make the parent a valid Rigidbody. Therefore, physics will not be applied on this collider.");
#endif
	}

} // namespace lyra