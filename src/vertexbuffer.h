#pragma once

#include "vertex.h"

namespace platz {

	class Vertexbuffer {
	public:

		std::vector<Vertex> vertices;

		Vertexbuffer(const std::vector<Vertex>& _vertices)
			: vertices(_vertices)
		{
		}
	};
}

