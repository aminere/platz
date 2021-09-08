
#include "pch.h"
#include "canvas.h"
#include "vector4.h"

namespace platz {

	Canvas::Canvas(int width, int height, int bpp /*= 3*/) 
		: _width(width)
		, _height(height)
		, _bpp(bpp)
	{
	}

	void Canvas::drawTriangle(
		const Vertex& v1,
		const Vertex& v2,
		const Vertex& v3,
		const zmath::Matrix44& mvp
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

		zmath::Vector3 coords;
		zmath::Triangle triangle(a.xyz, b.xyz, c.xyz);
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

					//const auto _u = coords.x * au + coords.y * bu + coords.z * cu;
					//const auto _v = coords.x * av + coords.y * bv + coords.z * cv;
					//const auto w = coords.x * aw + coords.y * bw + coords.z * cw;
					//const auto u = _u / w;
					//const auto v = _v / w;
					//const auto tx = std::min((int)(u * imageWidth), imageWidth - 1);
					//const auto ty = std::min((int)(v * imageHeight), imageHeight - 1);
					//const auto idx = ty * imageWidth * imageChannels + tx * imageChannels;
					//auto _r = imageData[idx];
					//auto _g = imageData[idx + 1];
					//auto _b = imageData[idx + 2];
					//Color c = { (unsigned char)_r, (unsigned char)_g, (unsigned char)_b };
					//drawPixel(context, _j, _i, c);
					
					drawPixel(_j, _i, Color::white);
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

