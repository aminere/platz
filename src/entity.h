#pragma once

#include <utility>
#include <vector>

#include "component.h"

#include <unordered_map>

#include "object.h"
#include "component.h"

namespace platz {	

	class Entity : public Object {

		DECLARE_OBJECT(Entity, Object);
		friend class Entities;
		friend class Components;

	private:

		std::unordered_map<int, std::shared_ptr<Component>> _components;

		Entity() = default;

	public:

		template <typename T, typename... Args>
		Entity* setComponent(Args&&... args) {
			const auto component = std::make_shared<T>(std::forward<Args>(args)...);
			component.get()->_entity = this;
			_components[T::TypeID] = component;
			return this;
		}

		template <typename T>
		T* getComponent() const {
			return static_cast<T*>(getComponentByTypeId(T::TypeID));
		}

		Component* getComponentByTypeId(int typeId) const {
			try {
				return _components.at(typeId).get();
			} catch (const std::exception& e) {
				(void)e;
				return nullptr;
			}
		}

		template <typename T>
		void clearComponent() {
			_components.erase(T::TypeID);
		}
	};
}
