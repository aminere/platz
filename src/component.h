#pragma once

#include "object.h"

namespace platz {
	class Entity;

	class Component : public Object {
		DECLARE_OBJECT(Component, Object);
		friend class Entity;

	private:

		Entity* _entity;

	public:

		inline Entity* entity() const { return _entity; }
	};
}
