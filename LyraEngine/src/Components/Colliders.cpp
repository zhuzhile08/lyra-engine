#include <ECS/colliders.h>



namespace lyra {

BoxCollider::BoxCollider(
		Rigidbody* parent,
		lsd::StringView name,
		const glm::vec3& dimensions,
		bool active,
		uint32 tag,
		const glm::vec3& position,
		const glm::vec3& rotation,
		const glm::vec3& scale,
		const RotationOrder rotationOrder
	) : Collider(parent, name, tag, active, position, rotation, scale, rotationOrder), 
		_dimensions(dimensions) { 
		log::warning("The parent Rigidbody of the BoxCollider at address: {} has been specified as a nullptr. Please make the parent a valid Rigidbody. Therefore, physics will not be applied on this collider.", lsd::getAddress(this));
	}

} // namespace lyra