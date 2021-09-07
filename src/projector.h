#pragma once

#include "matrix44.h"
#include "transform.h"

namespace platz {
	class Projector {

	public:

		float znear = 0.1f;
		float zfar = 100.0f;

		Projector(float _znear, float _zfar)
			: znear(_znear)
			, zfar(_zfar) {
		}

		virtual zmath::Matrix44 getProjectionMatrix() const = 0;

		virtual ~Projector() = default;
	};
}

