
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

	Frustum PerspectiveProjector::getFrustum(Transform* transform, float ratio) const {
		auto fovBy2 = zmath::radians(fov) / 2.f;
		auto hNear = tan(fovBy2) * znear;
		auto wNear = hNear * ratio;
		auto hFar = tan(fovBy2) * zfar;
		auto wFar = hFar * ratio;
		return Frustum(
			transform,
			wNear,
			hNear,
			wFar,
			hFar,
			znear,
			zfar
		);
	}
}

