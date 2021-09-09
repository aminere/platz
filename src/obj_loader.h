#pragma once

#include "vertexbuffer.h"

namespace platz {
	class OBJLoader {
	public:

		static Vertexbuffer* load(const std::string& path);
	};
}
