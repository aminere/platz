#pragma once

#include <utility>
#include <vector>

#include "component.h"

#include <unordered_map>
#include <memory>

#include "object.h"

namespace platz {	

	class Entity : public Object {

		DECLARE_OBJECT(Entity, Object);

	private:

		std::unordered_map<int, std::shared_ptr<Component>> _components;

	public:

		template <typename T, typename... Args>
		Entity* setComponent(Args&&... args) {
			_components[T::TypeID] = std::make_shared<T>(std::forward<Args>(args)...);
			return this;
		}

		template <typename T>
		T* getComponent() {
			try {
				return static_cast<T*>(_components.at(T::TypeID).get());				
			} catch (const std::exception& e) {
				(void)e;
				return nullptr;
			}			
		}
	};
}
