
#include "pch.h"
#include "components.h"

namespace platz {

	std::unordered_map<int, std::vector<Component*>> Components::_components;

	void Components::extract() {
		_components.clear();
		for (auto& entity : Entities::_entities) {
			for (auto& component : entity.second->_components) {
				auto componentPtr = component.second.get();
				componentPtr->preUpdate();
				_components[componentPtr->GetTypeID()].push_back(componentPtr);
			}
		}
	}
}
