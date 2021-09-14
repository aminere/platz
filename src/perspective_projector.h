#pragma once

#include "projector.h"
#include "zmath.h"

namespace platz {
	class PerspectiveProjector : public Projector {

	public:

		float fov;

		PerspectiveProjector(float _fov, float znear, float zfar)
			: Projector(znear, zfar)
			, fov(_fov) {

		}

		zmath::Matrix44 getProjectionMatrix() const override;
		Frustum getFrustum(Transform* transform, float ratio) const override;
	};
}
