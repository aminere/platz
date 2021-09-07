#pragma once

#include "vertexbuffer.h"

namespace platz {
	class Geometry {
	public:

		virtual Vertexbuffer* getVertexBuffer() const = 0;

		virtual ~Geometry() = default;
	};
}
