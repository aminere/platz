
#include "pch.h"
#include "canvas.h"
#include "vector4.h"
#include "material.h"

namespace platz {

	Canvas::Canvas(int width, int height, int bpp /*= 3*/) 
		: _width(width)
		, _height(height)
		, _bpp(bpp)
	{
		onResize(width, height);
	}

	void Canvas::clear() {
		const auto pixelCount = _width * _height;
		const auto size = pixelCount * _bpp;
		memset(_pixels, 0, size);
		memcpy(_zbuffer, _emptyZbuffer, pixelCount * sizeof(float));
	}

	void Canvas::drawTriangle(
		const Vertex& v1,
		const Vertex& v2,
		const Vertex& v3,
		const zmath::Matrix44& mvp,
		Material* material
	) {
		const auto& a = project(v1.position, mvp);
		const auto& b = project(v2.position, mvp);
		const auto& c = project(v3.position, mvp);

		// Near / far plane clipping
		// TODO per-pixel clipping
		if (abs(a.xyz.z) > 1.f || abs(b.xyz.z) > 1.f || abs(c.xyz.z) > 1.f) {
			return;
		}

		// calculate bounding rectangle
		auto minX = std::min(a.xyz.x, std::min(b.xyz.x, c.xyz.x));
		auto minY = std::min(a.xyz.y, std::min(b.xyz.y, c.xyz.y));
		auto maxX = std::max(a.xyz.x, std::max(b.xyz.x, c.xyz.x));
		auto maxY = std::max(a.xyz.y, std::max(b.xyz.y, c.xyz.y));

		// clip to screen space
		minX = std::max(minX, 0.f);
		minY = std::max(minY, 0.f);
		maxX = std::min(maxX, (float)_width);
		maxY = std::min(maxY, (float)_height);

		// calculate texture coordinates needed for perspective correct mapping
		auto au = v1.uv.x / a.w;
		auto av = v1.uv.y / a.w;
		auto aw = 1.f / a.w;
		auto bu = v2.uv.x / b.w;
		auto bv = v2.uv.y / b.w;
		auto bw = 1.f / b.w;
		auto cu = v3.uv.x / c.w;
		auto cv = v3.uv.y / c.w;
		auto cw = 1.f / c.w;

		// Rasterize
		zmath::Vector3 coords;
		zmath::Triangle triangle(a.xyz, b.xyz, c.xyz);
		auto texture = material->diffuse.get();
		for (auto i = minY; i < maxY; ++i) {
			for (auto j = minX; j < maxX; ++j) {
				if (triangle.contains(zmath::Vector3::create(j, i, 0), coords)) {
					const auto _j = (int)j;
					const auto _i = (int)i;
					if (_j < 0 || _i < 0 || _j >= _width || _i >= _height) {
						continue;
					}

					const auto index = (_i * _width) + _j;
					const auto newZ = coords.x * a.xyz.z + coords.y * b.xyz.z + coords.z * c.xyz.z;
					const auto oldZ = _zbuffer[index];
					if (newZ > oldZ) {
						continue;
					}
					_zbuffer[index] = newZ;

					const auto _u = coords.x * au + coords.y * bu + coords.z * cu;
					const auto _v = coords.x * av + coords.y * bv + coords.z * cv;
					const auto w = coords.x * aw + coords.y * bw + coords.z * cw;
					const auto u = _u / w;
					const auto v = _v / w;
					const auto tx = std::min((int)(u * texture->width), texture->width - 1);
					const auto ty = std::min((int)(v * texture->height), texture->height - 1);
					const auto idx = ty * texture->width * texture->bpp + tx * texture->bpp;
					auto _r = texture->data()[idx];
					auto _g = texture->data()[idx + 1];
					auto _b = texture->data()[idx + 2];
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

	zmath::Vector4 Canvas::project(const zmath::Vector4& worldPos, const zmath::Matrix44& mvp) {
		auto ndc = mvp * worldPos;
		ndc.xyz = ndc.xyz / ndc.w;
		zmath::Vector4 screenPos(
			((ndc.x + 1.f) / 2.f * _width),
			((-ndc.y + 1.f) / 2.f * _height),
			ndc.z,
			ndc.w
		);
		return screenPos;
	}
}

