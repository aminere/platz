#pragma once

#include <utility>
#include <vector>

#include "component.h"

namespace platz {
	class Entity {

	private:

		std::vector<Component*> _components;

	public:

		template <typename T, typename... Args>
		Entity* setComponent(Args&&... args) {
			auto t = new T(std::forward<Args>(args)...);
			_components.push_back(t);
			return this;
		}

		template <typename T>
		T* getComponent() {
			return static_cast<T*>(_components[0]);
		}
	};
}
