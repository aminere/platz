#pragma once

#include "component.h"
#include "geometry.h"
#include "material.h"

namespace platz {

	class Visual : public Component {
		DECLARE_OBJECT(Visual, Component);

	public:

		std::shared_ptr<Geometry> geometry;
		std::shared_ptr<Material> material;
		bool receiveShadows = true;
		bool castShadows = true;

		Visual(const std::shared_ptr<Geometry>& _geometry, const std::shared_ptr<Material>& _material)
			: geometry(_geometry)
			, material(_material) {

		}
	};
}
