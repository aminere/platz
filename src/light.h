#pragma once

#include "component.h"

namespace platz {
	class Light : public Component {
		DECLARE_OBJECT(Light, Component);	

	public:

		float intensity = 1.f;

		Light(float _intensity = 1.f)
			: intensity(_intensity) {

		}
	};
}

