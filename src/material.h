#pragma once

#include "vertex.h"
#include "light.h"

namespace platz {
	class Material {
	public:

		virtual Color Shade(const Vertex& vertex, const std::vector<Light*>& lights) const = 0;
	};
}

