
#include "pch.h"
#include "renderer.h"

#include "components.h"
#include "visual.h"
#include "camera.h"
#include "triangle.h"
#include "canvas.h"

#include "transform.h"

namespace platz {

	zmath::Vector2 Renderer::screenSize;

	void Renderer::render() const {
		auto visuals = Components::ofType<Visual>();

		auto cameras = Components::ofType<Camera>();
		if (cameras.size() < 1) {
			return;
		}
		
		for (auto camera : cameras) {
			
			const auto& projectionView = camera->projector->getProjectionMatrix() * camera->getViewMatrix();
			for (auto visual : visuals) {

				auto transform = visual->entity()->getComponent<Transform>();
				auto mvp = projectionView * transform->getWorldMatrix();
				auto vb = visual->geometry->getVertexBuffer();
				for (size_t i = 0; i < vb->vertices.size(); i += 3) {
					const auto& a = vb->vertices[i];
					const auto& b = vb->vertices[i + 1];
					const auto& c = vb->vertices[i + 2];					
					_canvas->drawTriangle(a, b, c, mvp);
				}
			}
		}
	}
}

