
#include "pch.h"
#include "perspective_projector.h"
#include "renderer.h"

namespace platz {
	zmath::Matrix44 PerspectiveProjector::getProjectionMatrix() const {
		auto ratio = Renderer::screenSize.x / Renderer::screenSize.y;
		return zmath::Matrix44::makePerspective(zmath::radians(fov), ratio, znear, zfar);
	}
}
