#pragma once

#include "component.h"
#include "entities.h"

namespace platz {
	class Components {

		static std::unordered_map<int, std::vector<Component*>> _components;

	public:
		template <class T>
		static std::vector<T*> ofType() {
			auto components = _components[T::TypeID];
			auto pointers = reinterpret_cast<T**>(components.data());
			return std::vector<T*>(pointers, pointers + components.size());
		}

		static void extract();
	};
}
