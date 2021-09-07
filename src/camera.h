#pragma once

#include "component.h"
#include "projector.h"

namespace platz {
	class Camera : public Component {
		DECLARE_OBJECT(Camera, Component);
	public:
		std::unique_ptr<Projector> projector;
		
		Camera(Projector* _projector)
		: projector(_projector) {

		}
	};
}
