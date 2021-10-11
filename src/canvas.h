#pragma once

#include "triangle.h"
#include "matrix44.h"
#include "vertex.h"
#include "shading_context.h"

namespace platz {

	class Material;
	class Light;

	class Canvas {

	public:

		Canvas(int width, int height, int bpp = 3);

		void clear();

		void drawTriangle(
			const ShadingContext& context,
			const std::vector<Vertex>& vertices,
			const zmath::Matrix44& projectionView,
			Material* material
		);

		void drawPixel(int x, int y, const Color& color);
		void drawPixel(int x, int y, unsigned char r, unsigned char g, unsigned char b);
		void drawLine(float x0, float y0, float x1, float y1, const Color& color);
		void drawLine(const zmath::Vector3& a, const zmath::Vector3& b, const Color& color);

		void onResize(int width, int height);

		inline int width() const { return _width; }
		inline int height() const { return _height; }
		inline int bpp() const { return _bpp; }
		inline unsigned char* pixels() const { return _pixels; }

	private:

		unsigned char* _pixels;
		float* _zbuffer;
		float* _emptyZbuffer;
		int _width;
		int _height;
		int _bpp;
	};
}
