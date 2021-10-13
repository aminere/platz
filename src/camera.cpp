
#include "pch.h"
#include "camera.h"
#include "entity.h"
#include "transform.h"
#include "engine.h"
#include "canvas.h"

namespace platz {
	DEFINE_OBJECT(Camera);

	zmath::Matrix44 Camera::getViewMatrix() const {
		zmath::Matrix44 viewMatrix;
		if (entity()->getComponent<Transform>()->worldMatrix().getInverse(viewMatrix)) {
			return viewMatrix;
		}
		return Matrix44::identity;
	}

	Frustum Camera::getFrustum() const {
		auto canvas = Engine::instance()->canvas();
		auto ratio = (float)canvas->width() / canvas->height();
		return projector->getFrustum(entity()->getComponent<Transform>(), ratio);
	}
}
