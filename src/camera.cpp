
#include "pch.h"
#include "camera.h"
#include "entity.h"
#include "transform.h"

namespace platz {
	DEFINE_OBJECT(Camera);

	zmath::Matrix44 Camera::getViewMatrix() const {
		zmath::Matrix44 viewMatrix;
		if (entity()->getComponent<Transform>()->getWorldMatrix().getInverse(viewMatrix)) {
			return viewMatrix;
		}
		return Matrix44::identity;
	}
}
