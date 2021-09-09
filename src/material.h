#pragma once

#include "texture.h"

namespace platz {
	class Material {
	public:

		std::shared_ptr<Texture> diffuse;

		Material(const std::shared_ptr<Texture>& _diffuse)
			: diffuse(_diffuse) {

		}
	};
}

