#include <Entity/ECS.h>

#include <Common/Logger.h>
#include <Common/UniquePointer.h>
#include <Common/Vector.h>

#include <Entity/Component.h>
#include <Entity/Entity.h>

#include <Common/UnorderedSparseMap.h>
#include <algorithm>

namespace lyra {

namespace ecs {

EntityComponentSystem* globalECS = nullptr;

}


void initECS() {
	if (ecs::globalECS)
		log::error("initECS(): The entity component system is already initialized!");
	else
		ecs::globalECS = new EntityComponentSystem();
}

} // namespace lyra
