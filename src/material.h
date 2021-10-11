#pragma once

#include "vertex.h"
#include "shading_context.h"

namespace platz {

	class Material {
	public:

		virtual Color shade(const ShadingContext& context, const Vertex& vertex) const = 0;
	};
}

