#pragma once

#include "entity.h"

namespace platz {
	class Entities {

		friend class Components;
		static std::unordered_map<int, std::unique_ptr<Entity>> _entities;

	public:

		static Entity* create();
	};
}
