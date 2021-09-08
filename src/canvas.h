#pragma once

#include "triangle.h"
#include "matrix44.h"
#include "vertex.h"

namespace platz {
	class Canvas {
	public:

		Canvas(int width, int height, int bpp = 3);

		~Canvas() {
			void(0);
		}

		void drawTriangle(
			const Vertex& a, 
			const Vertex& b,
			const Vertex& c,
			const zmath::Matrix44& mvp
		);

		void drawPixel(int x, int y, const Color& color);

		inline int width() const { return _width; }
		inline int height() const { return _height; }

	private:

		zmath::Vector4 project(const zmath::Vector4& worldPos, const zmath::Matrix44& mvp);

		unsigned char* _pixels;
		float* _zbuffer;
		int _width;
		int _height;
		int _bpp;		
	};
}
