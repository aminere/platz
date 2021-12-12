
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
		const ShadingContext& context,
		const std::vector<Vertex>& vertices,
		const zmath::Matrix44& projectionView,
		Material* material
	) {		

		zmath::Vector4 clipSpace[3];
		zmath::Vector3 ndc[3];
		for (int i = 0; i < 3; ++i) {
			// clip space position
			clipSpace[i] = projectionView * vertices[i].position;			

			// perspective division
			ndc[i] = zmath::Vector3(clipSpace[i].xyz / clipSpace[i].w);
		}		

		// back face culling
		auto normal = (ndc[1] - ndc[0]).cross(ndc[2] - ndc[0]);
		if (normal.z < 0.f) {
			return;
		}

		zmath::Vector3 screenSpace[3];
		for (int i = 0; i < 3; ++i) {			
			screenSpace[i] = zmath::Vector3(
				(ndc[i].x + 1.f) / 2.f * _width,
				(-ndc[i].y + 1.f) / 2.f * _height,
				ndc[i].z
			);
		}

		// calculate bounding rectangle
		auto fminX = std::min(screenSpace[0].x, std::min(screenSpace[1].x, screenSpace[2].x));
		auto fminY = std::min(screenSpace[0].y, std::min(screenSpace[1].y, screenSpace[2].y));
		auto fmaxX = std::max(screenSpace[0].x, std::max(screenSpace[1].x, screenSpace[2].x));
		auto fmaxY = std::max(screenSpace[0].y, std::max(screenSpace[1].y, screenSpace[2].y));

		// clip to screen space
		auto minX = std::max(0, std::min((int)std::floor(fminX), _width - 1));
		auto minY = std::max(0, std::min((int)std::floor(fminY), _height - 1));
		auto maxX = std::max(0, std::min((int)std::floor(fmaxX), _width - 1));
		auto maxY = std::max(0, std::min((int)std::floor(fmaxY), _height - 1));

		// calculate coordinates needed for perspective correct mapping
		auto at = zmath::Vector3(vertices[0].uv, 1.f) / clipSpace[0].w;
		auto bt = zmath::Vector3(vertices[1].uv, 1.f) / clipSpace[1].w;
		auto ct = zmath::Vector3(vertices[2].uv, 1.f) / clipSpace[2].w;
		auto ap = zmath::Vector4(vertices[0].position.xyz, 1.f) / clipSpace[0].w;
		auto bp = zmath::Vector4(vertices[1].position.xyz, 1.f) / clipSpace[1].w;
		auto cp = zmath::Vector4(vertices[2].position.xyz, 1.f) / clipSpace[2].w;
		auto an = zmath::Vector4(vertices[0].normal, 1.f) / clipSpace[0].w;
		auto bn = zmath::Vector4(vertices[1].normal, 1.f) / clipSpace[1].w;
		auto cn = zmath::Vector4(vertices[2].normal, 1.f) / clipSpace[2].w;
		auto ac = zmath::Vector4(vertices[0].color.r, vertices[0].color.g, vertices[0].color.b, 1.f) / clipSpace[0].w;
		auto bc = zmath::Vector4(vertices[1].color.r, vertices[1].color.g, vertices[1].color.b, 1.f) / clipSpace[1].w;
		auto cc = zmath::Vector4(vertices[2].color.r, vertices[2].color.g, vertices[2].color.b, 1.f) / clipSpace[2].w;

		// Rasterize
		zmath::Vector3 coords;
		zmath::Triangle triangle(
			zmath::Vector3(screenSpace[0].x, screenSpace[0].y, 0.f),
			zmath::Vector3(screenSpace[1].x, screenSpace[1].y, 0.f),
			zmath::Vector3(screenSpace[2].x, screenSpace[2].y, 0.f)
		);
		
		const auto stride = _width * _bpp;
		for (auto i = minY; i <= maxY; ++i) {			
			for (auto j = minX; j <= maxX; ++j) {	
				auto point = zmath::Vector3(.5f + j, .5f + i, 0);
				if (triangle.getBarycentricCoords(point, coords) && triangle.containsCoords(coords)) {
					const auto index = (i * _width) + j;
					const auto newZ = coords.x * screenSpace[0].z + coords.y * screenSpace[1].z + coords.z * screenSpace[2].z;
					const auto oldZ = _zbuffer[index];
					if (newZ > oldZ) {
						continue;
					}
					_zbuffer[index] = newZ;

					const auto wt = coords.x * at.z + coords.y * bt.z + coords.z * ct.z;
					zmath::Vector2 uv(
						abs((coords.x * at.x + coords.y * bt.x + coords.z * ct.x) / wt),
						abs((coords.x * at.y + coords.y * bt.y + coords.z * ct.y) / wt)
					);					

					const auto wp = coords.x * ap.w + coords.y * bp.w + coords.z * cp.w;
					zmath::Vector4 position(
						(coords.x * ap.x + coords.y * bp.x + coords.z * cp.x) / wp,
						(coords.x * ap.y + coords.y * bp.y + coords.z * cp.y) / wp,
						(coords.x * ap.z + coords.y * bp.z + coords.z * cp.z) / wp,
						1.f
					);

					const auto wn = coords.x * an.w + coords.y * bn.w + coords.z * cn.w;
					zmath::Vector3 normal(
						(coords.x * an.x + coords.y * bn.x + coords.z * cn.x) / wn,
						(coords.x * an.y + coords.y * bn.y + coords.z * cn.y) / wn,
						(coords.x * an.z + coords.y * bn.z + coords.z * cn.z) / wn
					);

					Color vertexColor(
						(coords.x * ac.x + coords.y * bc.x + coords.z * cc.x) / wn,
						(coords.x * ac.y + coords.y * bc.y + coords.z * cc.y) / wn,
						(coords.x * ac.z + coords.y * bc.z + coords.z * cc.z) / wn
					);

					auto color = material->shade(
						context,
						{
							position,
							uv,
							normal.normalized(),
							vertexColor
						}
					);

					// Draw pixel
					auto pixelIndex = (i * stride) + j * _bpp;
					_pixels[pixelIndex + 0] = (unsigned char)(color.r * 255.f);
					_pixels[pixelIndex + 1] = (unsigned char)(color.g * 255.f);
					_pixels[pixelIndex + 2] = (unsigned char)(color.b * 255.f);
				}
			}
		}
	}

	void Canvas::drawPixel(int x, int y, const Color& color) {		
		const auto stride = _width * _bpp;
		const auto index = (y * stride) + x * _bpp;
		_pixels[index + 0] = (unsigned char)(color.r * 255.f);
		_pixels[index + 1] = (unsigned char)(color.g * 255.f);
		_pixels[index + 2] = (unsigned char)(color.b * 255.f);
	}

	void Canvas::drawPixel(int x, int y, unsigned char r, unsigned char g, unsigned char b) {
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

