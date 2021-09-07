#pragma once

#include "component.h"
#include "projector.h"

namespace platz {
	class Camera : public Component {
		std::unique_ptr<Projector> _projector;
	};
}
