
#include "pch.h"

#include "entities.h"

namespace platz {
	std::unordered_map<int, std::unique_ptr<Entity>> Entities::_entities;

	Entity* Entities::create() {
		auto size = (int)_entities.size();
		auto entity = new Entity();
		_entities.insert({ size, std::unique_ptr<Entity>(entity) });
		return entity;
	}
}
