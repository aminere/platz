#pragma once

#include "matrix44.h"

namespace platz {
	class Projector {
		float _zNear = 0.1f;
		float _zFar = 100.0f;

	public:

		virtual zmath::Matrix44 getProjecionMatrix() const {
			return zmath::Matrix44::identity;
		}		
	};
}
