#pragma once

#include "vector4.h"
#include "vector3.h"
#include "vector2.h"
#include "color.h"

namespace platz {
	struct Vertex {
		zmath::Vector4 position;
		zmath::Vector2 uv;
		zmath::Vector3 normal;
		Color color;

		Vertex(
			const zmath::Vector4& _position,
			const zmath::Vector2& _uv,
			const zmath::Vector3& _normal,
			const Color& _color
		) 
		: position(_position)
		, uv(_uv)
		, normal(_normal)
		, color(_color)
		{

		}
	};
}

