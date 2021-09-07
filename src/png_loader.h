
#pragma once

#include <string>

namespace platz {
	class PNGLoader {
	public:
		static unsigned char* load(const std::string& path, int& width, int& height, int& channels);
	};	
}
