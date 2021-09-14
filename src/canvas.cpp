
#include "pch.h"
#include "canvas.h"
#include "vector4.h"
#include "material.h"
#include "vector3.h"
#include "plane.h"
#include "clipping.h"

#include <assert.h>

namespace platz {

	Canvas::Canvas(int width, int height, int bpp /*= 3*/)
		: _width(width)
		, _height(height)
		, _bpp(bpp) {
		onResize(width, height);
	}

	void Canvas::clear() {
		const auto pixelCount = _width * _height;
		const auto size = pixelCount * _bpp;
		memset(_pixels, 0, size);
		memcpy(_zbuffer, _emptyZbuffer, pixelCount * sizeof(float));
	}

	void Canvas::drawTriangle(
		const std::vector<Vertex>& vertices,
		const zmath::Matrix44& mvp,
		Material* material
	) {
		// clip space position
		zmath::Vector4 clipSpace[3];
		for (int i = 0; i < 3; ++i) {
			clipSpace[i] = mvp * vertices[i].position;
		}

		// back face culling
		auto normal = (clipSpace[1].xyz - clipSpace[0].xyz).cross(clipSpace[2].xyz - clipSpace[0].xyz);
		if (normal.z < 0.f) {
			return;
		}

		// perspective division
		zmath::Vector3 ndc[3];
		for (int i = 0; i < 3; ++i) {
			ndc[i] = zmath::Vector3(clipSpace[i].xyz / clipSpace[i].w);
		}		

		// convert to screen space
		zmath::Vector3 screenCoord[3];
		for (int i = 0; i < 3; ++i) {
			screenCoord[i] = zmath::Vector3(
				(ndc[i].x + 1.f) / 2.f * _width,
				(-ndc[i].y + 1.f) / 2.f * _height,
				ndc[i].z
			);
		}

		// calculate bounding rectangle
		auto minX = std::min(screenCoord[0].x, std::min(screenCoord[1].x, screenCoord[2].x));
		auto minY = std::min(screenCoord[0].y, std::min(screenCoord[1].y, screenCoord[2].y));
		auto maxX = std::max(screenCoord[0].x, std::max(screenCoord[1].x, screenCoord[2].x));
		auto maxY = std::max(screenCoord[0].y, std::max(screenCoord[1].y, screenCoord[2].y));

		// clip to screen space
		minX = std::max(minX, 0.f);
		minY = std::max(minY, 0.f);
		maxX = std::min(maxX, (float)_width);
		maxY = std::min(maxY, (float)_height);

		// calculate texture coordinates needed for perspective correct mapping
		auto at = zmath::Vector3(vertices[0].uv, 1.f) / clipSpace[0].w;
		auto bt = zmath::Vector3(vertices[1].uv, 1.f) / clipSpace[1].w;
		auto ct = zmath::Vector3(vertices[2].uv, 1.f) / clipSpace[2].w;

		// Rasterize
		zmath::Vector3 coords;
		zmath::Triangle triangle(screenCoord[0], screenCoord[1], screenCoord[2]);
		auto texture = material->diffuse.get();
		for (auto i = minY; i < maxY; ++i) {
			for (auto j = minX; j < maxX; ++j) {
				if (triangle.contains(zmath::Vector3(j, i, 0), coords)) {
					const auto _j = (int)j;
					const auto _i = (int)i;
					if (_j < 0 || _i < 0 || _j >= _width || _i >= _height) {
						continue;
					}

					const auto index = (_i * _width) + _j;
					const auto newZ = coords.x * screenCoord[0].z + coords.y * screenCoord[1].z + coords.z * screenCoord[2].z;
					const auto oldZ = _zbuffer[index];
					if (newZ > oldZ) {
						continue;
					}
					_zbuffer[index] = newZ;

					const auto _u = coords.x * at.x + coords.y * bt.x + coords.z * ct.x;
					const auto _v = coords.x * at.y + coords.y * bt.y + coords.z * ct.y;
					const auto w = coords.x * at.z + coords.y * bt.z + coords.z * ct.z;
					const auto u = abs(_u / w);
					const auto v = abs(_v / w);
					const auto tx = std::min((int)(u * texture->width), texture->width - 1);
					const auto ty = std::min((int)(v * texture->height), texture->height - 1);
					const auto idx = ty * texture->width * texture->bpp + tx * texture->bpp;
					const auto _r = texture->data()[idx];
					const auto _g = texture->data()[idx + 1];
					const auto _b = texture->data()[idx + 2];
					drawPixel(_j, _i, _r, _g, _b);					
				}
			}
		}
	}

	void Canvas::drawPixel(int x, int y, const Color& color) {
		if (x < 0 || y < 0 || x >= _width || y >= _height) {
			return;
		}
		const auto stride = _width * _bpp;
		const auto index = (y * stride) + x * _bpp;
		_pixels[index + 0] = (unsigned char)(color.r * 255.f);
		_pixels[index + 1] = (unsigned char)(color.g * 255.f);
		_pixels[index + 2] = (unsigned char)(color.b * 255.f);
	}

	void Canvas::drawPixel(int x, int y, unsigned char r, unsigned char g, unsigned char b) {
		if (x < 0 || y < 0 || x >= _width || y >= _height) {
			return;
		}
		const auto stride = _width * _bpp;
		const auto index = (y * stride) + x * _bpp;
		_pixels[index + 0] = r;
		_pixels[index + 1] = g;
		_pixels[index + 2] = b;
	}

	void Canvas::drawLine(float x0, float y0, float x1, float y1, const Color& color) {
		// Based on https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
		auto ix1 = (int)x1;
		auto iy1 = (int)y1;
		auto ix0 = (int)x0;
		auto iy0 = (int)y0;
		const auto dx = abs(ix1 - ix0);
		const auto sx = ix0 < ix1 ? 1 : -1;
		const auto dy = -abs(iy1 - iy0);
		const auto sy = iy0 < iy1 ? 1 : -1;
		auto err = dx + dy;
		while (true) {
			drawPixel(ix0, iy0, color);
			if (ix0 == ix1 && iy0 == iy1) {
				break;
			}
			const auto e2 = 2 * err;
			if (e2 >= dy) {
				err += dy;
				ix0 += sx;
			}
			if (e2 <= dx) {
				err += dx;
				iy0 += sy;
			}
		}
	}

	void Canvas::drawLine(const zmath::Vector3& a, const zmath::Vector3& b, const Color& color) {
		drawLine(a.x, a.y, b.x, b.y, color);
	}

	void Canvas::onResize(int width, int height) {
		_width = width;
		_height = height;

		if (_pixels) {
			delete[] _pixels;
			delete[] _zbuffer;
			delete[] _emptyZbuffer;
		}

		const auto pixelCount = width * height;
		_pixels = new unsigned char[(size_t)pixelCount * _bpp];
		_zbuffer = new float[pixelCount];
		_emptyZbuffer = new float[pixelCount];
		for (int i = 0; i < pixelCount; ++i) {
			_emptyZbuffer[i] = 1.0f;
		}
	}
}

