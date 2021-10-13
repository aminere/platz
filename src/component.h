#pragma once

#include "object.h"

namespace platz {
	class Entity;

	class Component : public Object {
		DECLARE_OBJECT(Component, Object);
		friend class Entity;

	private:

		Entity* _entity = nullptr;

	public:

		virtual ~Component() = default;

		virtual void preUpdate() {}

		inline Entity* entity() const { return _entity; }
	};
}
