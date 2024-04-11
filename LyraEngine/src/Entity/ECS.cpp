#include <Entity/ECS.h>

#include <Common/Logger.h>
#include <Common/UniquePointer.h>
#include <Common/Vector.h>

#include <Entity/Component.h>
#include <Entity/Entity.h>

#include <Common/UnorderedSparseMap.h>
#include <algorithm>

namespace lyra {

namespace {

static EntityComponentSystem* globalECS;

}

void initECS() {
	if (globalECS) {
		log::error("initECS(): The entity component system is already initialized!");
		return;
	}

	globalECS = new EntityComponentSystem();
}

namespace ecs {

EntityComponentSystem* defaultECS() {
	return globalECS;
}

} // namespace ecs

} // namespace lyra
