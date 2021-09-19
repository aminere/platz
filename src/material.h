#pragma once

#include "vertex.h"
#include "light.h"

namespace platz {
	class Material {
	public:

		virtual Color shade(const Vertex& vertex, const std::vector<Light*>& lights) const = 0;
	};
}

