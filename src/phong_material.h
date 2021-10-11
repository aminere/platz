#pragma once
#pragma once

#include "material.h"
#include "texture.h"

namespace platz {
	class PhongMaterial : public Material  {
	public:		

		PhongMaterial(
			const Color& ambient, 
			const std::shared_ptr<Texture>& diffuse = nullptr, 
			float specular = 2.f
		);

		virtual Color shade(const ShadingContext& context, const Vertex& vertex) const override;

	private:

		Color _ambient;		
		std::shared_ptr<Texture> _diffuse;
		float _specular = 2.f;
	};
}

