#include <ECS/ECS.h>

#include <Common/Logger.h>

namespace lyra {

namespace ecs {

World* globalECSWorld = nullptr;

void update() {
	
}

} // namespace ecs

void initECS() {
	if (ecs::globalECSWorld)
		log::error("initECS(): The entity component system is already initialized!");
	else
		ecs::globalECSWorld = new ecs::World();
}

} // namespace lyra
