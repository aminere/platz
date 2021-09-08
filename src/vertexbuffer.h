#pragma once

#include "vertex.h"

namespace platz {

	class Vertexbuffer {
	public:

		std::vector<Vertex> vertices;

		Vertexbuffer(Vertex _vertices[]) {
			auto size = sizeof(_vertices) / sizeof(_vertices[0]);
			vertices = std::vector<Vertex>(_vertices, _vertices + size);			
		}
	};
}

