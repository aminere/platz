
#include "pch.h"
#include "phong_material.h"
#include "entity.h"
#include "transform.h"

namespace platz {

	PhongMaterial::PhongMaterial(const Color& ambient, const std::shared_ptr<Texture>& diffuse)
		: _ambient(ambient)
		, _diffuse(diffuse)
	{
	}

	Color PhongMaterial::shade(const Vertex& vertex, const std::vector<Light*>& lights) const {
		Color diffuse;
		if (auto diffuseTex = _diffuse.get()) {
			const auto tx = (int)(vertex.uv.x * diffuseTex->width) % diffuseTex->width;
			const auto ty = (int)(vertex.uv.y * diffuseTex->height) % diffuseTex->height;
			const auto idx = ty * diffuseTex->width * diffuseTex->bpp + tx * diffuseTex->bpp;
			diffuse = {
				(float)diffuseTex->data()[idx] / 255.f,
				(float)diffuseTex->data()[idx + 1] / 255.f,
				(float)diffuseTex->data()[idx + 2] / 255.f,
				1.f
			};
		}

		auto lightAmount = .0f;
		for (auto& light : lights) {
			auto lightTransform = light->entity()->getComponent<Transform>();
			auto lightDir = lightTransform->worldForward();
			lightAmount += std::min(std::max(lightDir.dot(vertex.normal), 0.f), 1.f);
		}
		
		return _ambient + diffuse * lightAmount;
	}
}
