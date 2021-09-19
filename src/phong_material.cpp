
#include "pch.h"
#include "phong_material.h"
#include "entity.h"
#include "transform.h"

namespace platz {

	PhongMaterial::PhongMaterial(
		const Color& ambient, 
		const std::shared_ptr<Texture>& diffuse, 
		float specular
	)
		: _ambient(ambient)
		, _diffuse(diffuse)
		, _specular(specular)
	{
	}

	Color PhongMaterial::shade(const Vertex& vertex, const zmath::Vector3& viewPos, const std::vector<Light*>& lights) const {
		Vector3 albedo = Vector3::zero;
		if (auto diffuseTex = _diffuse.get()) {
			const auto tx = (int)(vertex.uv.x * diffuseTex->width) % diffuseTex->width;
			const auto ty = (int)(vertex.uv.y * diffuseTex->height) % diffuseTex->height;
			const auto idx = ty * diffuseTex->width * diffuseTex->bpp + tx * diffuseTex->bpp;
			albedo = {
				(float)diffuseTex->data()[idx],
				(float)diffuseTex->data()[idx + 1],
				(float)diffuseTex->data()[idx + 2]				
			};
		}

		Vector3 diffuse = Vector3::zero;
		float specular = 0.f;
		auto viewDir = -viewPos.normalized();
		for (auto& light : lights) {
			auto lightDir = light->entity()->getComponent<Transform>()->worldForward();
			auto lightFactor = std::max(-lightDir.dot(vertex.normal), 0.f);
			diffuse = diffuse + albedo * light->intensity * lightFactor;

			auto reflected = lightDir.reflect(vertex.normal);
			specular = specular + light->intensity * std::pow(std::max(0.f, reflected.dot(viewDir)), _specular);
		}
		
		auto _diffuse = diffuse / 255.f;
		return _ambient + Color(_diffuse.x, _diffuse.y, _diffuse.z) + Color::white * specular;
	}
}
