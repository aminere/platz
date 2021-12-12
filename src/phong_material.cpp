
#include "pch.h"
#include "phong_material.h"
#include "entity.h"
#include "transform.h"
#include "light.h"
#include "ray.h"
#include "visual.h"
#include "triangle.h"
#include "collision.h"

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

	Color PhongMaterial::shade(const ShadingContext& context, const Vertex& vertex) const {
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
		auto specular = 0.f;
		auto lightFactor = 1.f;
		auto viewDir = (context.cameraPos - vertex.position.xyz).normalized();
		for (auto& light : context.lights) {
			auto lightDir = light->entity()->getComponent<Transform>()->forward();
			diffuse = diffuse + albedo * light->intensity * std::max(-lightDir.dot(vertex.normal), 0.f);

			auto reflected = lightDir.reflect(vertex.normal);
			specular = specular + light->intensity * std::pow(std::max(0.f, reflected.dot(viewDir)), _specular);

			if (context.receiveShadows) {
				auto inShadow = false;

				// Cast a ray towards the light
				// Start the ray a little bit outside the surface to avoid collisions with self
				auto surfacePos = vertex.position.xyz + vertex.normal * .01f;
				zmath::Ray ray(surfacePos, -lightDir);

				// Loop through shadow casters
				for (auto& visual : context.visuals) {
					if (!visual->castShadows) {
						continue;
					}
					auto vb = visual->geometry->getVertexBuffer();
					auto transform = visual->entity()->getComponent<Transform>();
					for (int i = 0; i < vb->vertices.size(); i += 3) {
						Triangle triangle(
							transform->worldMatrix() * vb->vertices[i].position.xyz,
							transform->worldMatrix() * vb->vertices[i + 1].position.xyz,
							transform->worldMatrix() * vb->vertices[i + 2].position.xyz
						);
						Collision::RayTriangleResult result;
						if (Collision::rayTriangle(ray, triangle, result)) {
							// Remove the influence of this light
							lightFactor -= 1.f / context.lights.size();
							inShadow = true;
							break;
						}
					}
					if (inShadow) {
						break;
					}
				}
			}
		}
		
		auto _diffuse = diffuse / 255.f;
		auto color = _ambient + Color(_diffuse.x, _diffuse.y, _diffuse.z) + Color::white * specular;
		return color * lightFactor;
	}
}
