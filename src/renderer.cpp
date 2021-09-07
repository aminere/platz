
#include "pch.h"
#include "renderer.h"

#include "components.h"
#include "visual.h"
#include "camera.h"

namespace platz {

	zmath::Vector2 Renderer::screenSize;

	void Renderer::render() const {
		auto visuals = Components::ofType<Visual>();

		auto cameras = Components::ofType<Camera>();
		if (cameras.size() < 1) {
			return;
		}

		// support one camera for now
		auto camera = cameras[0];

	}
}
