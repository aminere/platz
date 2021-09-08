
#include "pch.h"
#include "perspective_projector.h"

#include "engine.h"
#include "canvas.h"

namespace platz {
	zmath::Matrix44 PerspectiveProjector::getProjectionMatrix() const {
		auto canvas = Engine::instance()->canvas();
		auto ratio = (float)canvas->width() / canvas->height();
		return zmath::Matrix44::makePerspective(zmath::radians(fov), ratio, znear, zfar);
	}
}
