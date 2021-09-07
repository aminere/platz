
#include "pch.h"
#include "texture.h"

#include "png_loader.h"

namespace platz {
	void Texture::load() {
		_data = PNGLoader::load(path, width, height, bpp);
	}
}

