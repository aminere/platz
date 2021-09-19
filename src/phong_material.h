#pragma once
#pragma once

#include "material.h"
#include "texture.h"

namespace platz {
	class PhongMaterial : public Material  {
	public:		

		PhongMaterial(const Color& ambient, const std::shared_ptr<Texture>& diffuse = nullptr);

		virtual Color Shade(const Vertex& vertex, const std::vector<Light*>& lights) const override;

	private:

		Color _ambient;		
		std::shared_ptr<Texture> _diffuse;
	};
}

