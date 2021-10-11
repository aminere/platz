#pragma once

#include "vector3.h"
#include <vector>

namespace platz {

	class Visual;
	class Light;

	struct ShadingContext {
		zmath::Vector3 cameraPos;
		const std::vector<Visual*>& visuals;
		const std::vector<Light*>& lights;
		bool receiveShadows;
	};
}
