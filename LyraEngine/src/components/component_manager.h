#pragma once

#include <core/settings.h>
#include <core/logger.h>
#include <math/math.h>

#include <array>

namespace lyra {

template<class _Ty> class ComponentManger {
public:
	ComponentManager() { }
	
	const uint32 insertComponent(_Ty &components) {
		lassert(usedIndex < Settings::Memory::componentLimit, "Exceeded maximum entity limit(", Settings::Memory::componentLimit, ")!");

		_components.at(usedIndex) = std::move(components);
		usedIndex++;
		return usedIndex - 1;
	}

	/**
	 * @brief get the array with the components
	 * 
	 * @return const std::array<_Ty, Settings::Memory::componentLimit>* const
	 */
	const std::array<_Ty, Settings::Memory::componentLimit>* const components() { return &_components; }

private:
	std::array<_Ty, Settings::Memory::componentLimit> _components;
	uint32 usedIndex;
};

}
