#pragma once

#include "vector4.h"
#include "vector2.h"
#include "color.h"

namespace platz {

	struct Vertex {
		zmath::Vector4 position;
		zmath::Vector2 uv;
		Color color;
	};

	class Vertexbuffer {
	public:

		std::vector<Vertex> vertices;

		Vertexbuffer(Vertex _vertices[]) {
			auto size = sizeof(_vertices) / sizeof(_vertices[0]);
			vertices = std::vector<Vertex>(_vertices, _vertices + size);			
		}
	};
}

