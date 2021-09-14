#pragma once

#include "component.h"
#include "projector.h"
#include "matrix44.h"

namespace platz {
	class Camera : public Component {
		DECLARE_OBJECT(Camera, Component);

	public:
		std::unique_ptr<Projector> projector;
		
		Camera(Projector* _projector)
		: projector(_projector) {

		}

		zmath::Matrix44 getViewMatrix() const;
		Frustum getFrustum() const;
	};
}
