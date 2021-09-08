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
}
