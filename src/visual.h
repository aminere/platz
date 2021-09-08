#pragma once

#include "component.h"
#include "geometry.h"
#include "material.h"

namespace platz {

	class Visual : public Component {
		DECLARE_OBJECT(Visual, Component);

	public:

		std::unique_ptr<Geometry> geometry;
		std::unique_ptr<Material> material;

		Visual(Geometry* _geometry, Material* _material)
			: geometry(_geometry)
			, material(_material) {

		}
	};
}
