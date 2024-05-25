#include <ECS/ECS.h>

#include <Common/Logger.h>

namespace lyra {

namespace ecs {

World* globalWorld = nullptr;

void update() {
	globalWorld->update();
}

} // namespace ecs

void initECS() {
	if (ecs::globalWorld)
		log::error("initECS(): The entity component system is already initialized!");
	else
		ecs::globalWorld = new ecs::World();
}

} // namespace lyra
