
#include "pch.h"
#include "texture.h"

#include "png_loader.h"

namespace platz {
	Texture::Texture(const std::string& path) {
		_data = PNGLoader::load(path, width, height, bpp);
	}
}

