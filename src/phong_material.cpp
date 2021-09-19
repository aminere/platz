
#include "pch.h"
#include "phong_material.h"

namespace platz {

	PhongMaterial::PhongMaterial(const Color& ambient, const std::shared_ptr<Texture>& diffuse)
		: _ambient(ambient)
		, _diffuse(diffuse)
	{
	}

	Color PhongMaterial::Shade(const Vertex& vertex, const std::vector<Light*>& lights) const {
		Color color = _ambient;

		if (auto diffuse = _diffuse.get()) {
			const auto tx = (int)(vertex.uv.x * diffuse->width) % diffuse->width;
			const auto ty = (int)(vertex.uv.y * diffuse->height) % diffuse->height;
			const auto idx = ty * diffuse->width * diffuse->bpp + tx * diffuse->bpp;
			color = {
				(float)diffuse->data()[idx] / 255.f,
				(float)diffuse->data()[idx + 1] / 255.f,
				(float)diffuse->data()[idx + 2] / 255.f,
				1.f
			};
		}

		if (vertex.normal.z > 0.5f) {
			return Color::green;
		}
		
		return color;
	}
}
