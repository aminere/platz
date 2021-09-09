#pragma once

namespace platz {
	class Texture {
	public:

		int width = 0;
		int height = 0;
		int bpp = 0;

		Texture(const std::string& _path);

		inline unsigned char* data() const { return _data; }

	private:

		unsigned char* _data = nullptr;
	};
}

