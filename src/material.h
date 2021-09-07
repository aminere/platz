#pragma once

#include "texture.h"

namespace platz {
	class Material {
	public:

		std::unique_ptr<Texture> diffuse;

		Material(Texture* _diffuse)
			: diffuse(_diffuse) {

		}
	};
}

