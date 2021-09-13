
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

		// frustum clipping
		std::vector<zmath::Triangle> clippedTriangles;
		std::vector<int> inside;
		std::vector<int> outside;
		for (int i = 0; i < 3; ++i) {
			auto clipPos = clipSpace[i];
			if (abs(clipPos.x) > clipPos.w || abs(clipPos.y) > clipPos.w || abs(clipPos.z) > clipPos.w) {
				outside.push_back(i);
			} else {
				inside.push_back(i);
			}
		}

		if (outside.size() == 3) {
			// All vertices are clipped, discard the triangle entirely
			return;
		}

		if (inside.size() == 3) {
			// All vertices are inside, draw the triangle as is
			drawClippedTriangle(
				Vertex(clipSpace[0], vertices[0].uv, vertices[0].normal, vertices[0].color),
				Vertex(clipSpace[1], vertices[1].uv, vertices[1].normal, vertices[1].color),
				Vertex(clipSpace[2], vertices[2].uv, vertices[2].normal, vertices[2].color),
				mvp,
				material
			);

		} else if (outside.size() == 2) {
			
			zmath::Vector3 intersection1;
			for (int i = 0; i < 3; ++i) {
				auto outsidePos = clipSpace[outside[0]];
				auto coord = outsidePos.coords[i];
				if (abs(coord) > outsidePos.w) {
					auto insidePos = clipSpace[inside[0]];
					auto x1 = insidePos.coords[i];
					auto w1 = insidePos.w;
					auto x2 = coord;
					auto w2 = outsidePos.w;
					auto dx = x1 - x2;
					float w;
					if (dx == 0.f) {
						w = x1;
					} else {
						auto b = (x1 * w1 - x2 * w1) / dx;
						auto a = x1 != 0.f ? ((w1 - b) / x1) : ((w2 - b) / x2);
						w = b / (1.f - a);
					}
					intersection1.coords[i] = zmath::sign(coord) * abs(w);
				} else {
					intersection1.coords[i] = coord;
				}
			}

			zmath::Vector3 intersection2;
			for (int i = 0; i < 3; ++i) {
				auto outsidePos = clipSpace[outside[1]];
				auto coord = outsidePos.coords[i];
				if (abs(coord) > outsidePos.w) {
					auto insidePos = clipSpace[inside[0]];
					auto x1 = insidePos.coords[i];
					auto w1 = insidePos.w;
					auto x2 = coord;
					auto w2 = outsidePos.w;
					auto dx = x1 - x2;
					float w;
					if (dx == 0.f) {
						w = x1;
					} else {
						auto b = (x1 * w1 - x2 * w1) / dx;
						auto a = x1 != 0.f ? ((w1 - b) / x1) : ((w2 - b) / x2);
						w = b / (1.f - a);
					}
					intersection2.coords[i] = zmath::sign(coord) * abs(w);
				} else {
					intersection2.coords[i] = coord;
				}
			}

			//auto w1 = zmath::lerp(inside[0].w, outside[0].w, ((intersection1 - inside[0].xyz).length() / (outside[0].xyz - inside[0].xyz).length()));
			//auto w2 = zmath::lerp(inside[0].w, outside[1].w, ((intersection2 - inside[0].xyz).length() / (outside[1].xyz - inside[0].xyz).length()));
			//assert(w1 == outside[0].w);
			//assert(w2 == outside[1].w);
			drawClippedTriangle(
				Vertex(clipSpace[inside[0]], vertices[inside[0]].uv, vertices[inside[0]].normal, vertices[inside[0]].color),
				Vertex(
					zmath::Vector4(intersection1, clipSpace[outside[0]].w), 
					// TODO lerp between [inside[0] - outside[0]]
					vertices[outside[0]].uv, 
					vertices[outside[0]].normal,
					vertices[outside[0]].color
				),
				Vertex(
					zmath::Vector4(intersection2, clipSpace[outside[1]].w), 
					// TODO lerp between [inside[0] - outside[0]]
					vertices[outside[1]].uv,
					vertices[outside[1]].normal,
					vertices[outside[1]].color
				),
				mvp,
				material
			);

		} else {

			zmath::Vector3 intersection1;
			zmath::Vector3 intersection2;
			for (int i = 0; i < 3; ++i) {
				auto outsidePos = clipSpace[outside[0]];
				auto coord = outsidePos.coords[i];
				if (abs(coord) > outsidePos.w) {
					auto x2 = coord;
					auto w2 = outsidePos.w;

					{
						auto insidePos = clipSpace[inside[0]];
						auto x1 = insidePos.coords[i];
						auto w1 = insidePos.w;
						auto dx = x1 - x2;
						float w;
						if (dx == 0.f) {
							w = x1;
						} else {
							auto b = (x1 * w1 - x2 * w1) / dx;
							auto a = x1 != 0.f ? ((w1 - b) / x1) : ((w2 - b) / x2);
							w = b / (1.f - a);
						}
						intersection1.coords[i] = zmath::sign(coord) * abs(w);
					}
					
					{
						auto insidePos = clipSpace[inside[1]];
						auto x1 = insidePos.coords[i];
						auto w1 = insidePos.w;
						auto dx = x1 - x2;
						float w;
						if (dx == 0.f) {
							w = x1;
						} else {
							auto b = (x1 * w1 - x2 * w1) / dx;
							auto a = x1 != 0.f ? ((w1 - b) / x1) : ((w2 - b) / x2);
							w = b / (1.f - a);
						}
						intersection2.coords[i] = zmath::sign(coord) * abs(w);
					}

				} else {
					intersection1.coords[i] = coord;
					intersection2.coords[i] = coord;
				}
			}

			drawClippedTriangle(
				Vertex(clipSpace[inside[0]], vertices[inside[0]].uv, vertices[inside[0]].normal, vertices[inside[0]].color),
				Vertex(
					zmath::Vector4(intersection1, clipSpace[outside[0]].w), 
					// TODO lerp between [inside[0] - outside[0]]
					vertices[outside[0]].uv,
					vertices[outside[0]].normal,
					vertices[outside[0]].color
				),
				Vertex(
					zmath::Vector4(intersection2, clipSpace[outside[0]].w), 
					// TODO lerp between [inside[1] - outside[0]]
					vertices[outside[0]].uv,
					vertices[outside[0]].normal,
					vertices[outside[0]].color
				),
				mvp,
				material
			);

			drawClippedTriangle(
				Vertex(clipSpace[inside[0]], vertices[inside[0]].uv, vertices[inside[0]].normal, vertices[inside[0]].color),
				Vertex(
					zmath::Vector4(intersection2, clipSpace[outside[0]].w),
					// TODO lerp between [inside[1] - outside[0]]
					vertices[outside[0]].uv,
					vertices[outside[0]].normal,
					vertices[outside[0]].color
				),
				Vertex(clipSpace[inside[1]], vertices[inside[1]].uv, vertices[inside[1]].normal, vertices[inside[1]].color),				
				mvp,
				material
			);
		}
	}

	void Canvas::drawClippedTriangle(
		const Vertex& v1,
		const Vertex& v2,
		const Vertex& v3,
		const zmath::Matrix44& mvp,
		Material* material
	) {
		// perspective division
		zmath::Vector3 a(v1.position.xyz / v1.position.w);
		zmath::Vector3 b(v2.position.xyz / v2.position.w);
		zmath::Vector3 c(v3.position.xyz / v3.position.w);

		// back face culling
		//auto normal = (b.xyz - a.xyz).cross(c.xyz - a.xyz);
		//if (normal.z > 0.f) {
		//	return;
		//}

		// convert to screen space
		a.x = ((a.x + 1.f) / 2.f * _width);
		a.y = ((-a.y + 1.f) / 2.f * _height);
		b.x = ((b.x + 1.f) / 2.f * _width);
		b.y = ((-b.y + 1.f) / 2.f * _height);
		c.x = ((c.x + 1.f) / 2.f * _width);
		c.y = ((-c.y + 1.f) / 2.f * _height);

		// calculate bounding rectangle
		auto minX = std::min(a.x, std::min(b.x, c.x));
		auto minY = std::min(a.y, std::min(b.y, c.y));
		auto maxX = std::max(a.x, std::max(b.x, c.x));
		auto maxY = std::max(a.y, std::max(b.y, c.y));

		// clip to screen space
		minX = std::max(minX, 0.f);
		minY = std::max(minY, 0.f);
		maxX = std::min(maxX, (float)_width);
		maxY = std::min(maxY, (float)_height);

		// calculate texture coordinates needed for perspective correct mapping
		auto at = zmath::Vector3(v1.uv, 1.f) / v1.position.w;
		auto bt = zmath::Vector3(v2.uv, 1.f) / v2.position.w;
		auto ct = zmath::Vector3(v3.uv, 1.f) / v3.position.w;

		// Rasterize
		zmath::Vector3 coords;
		zmath::Triangle triangle(a, b, c);
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
					const auto newZ = coords.x * a.z + coords.y * b.z + coords.z * c.z;
					const auto oldZ = _zbuffer[index];
					if (newZ > oldZ) {
						continue;
					}
					_zbuffer[index] = newZ;

					const auto _u = coords.x * at.x + coords.y * bt.x + coords.z * ct.x;
					const auto _v = coords.x * at.y + coords.y * bt.y + coords.z * ct.y;
					const auto w = coords.x * at.z + coords.y * bt.z + coords.z * ct.z;
					auto u = _u / w;
					auto v = _v / w;

					// TODO support tiling
					//u = std::max(std::min(u, 1.f), 0.f);
					//v = std::max(std::min(v, 1.f), 0.f);

					const auto tx = std::min((int)(u * texture->width), texture->width - 1);
					const auto ty = std::min((int)(v * texture->height), texture->height - 1);
					const auto idx = ty * texture->width * texture->bpp + tx * texture->bpp;
					auto _r = texture->data()[idx];
					auto _g = texture->data()[idx + 1];
					auto _b = texture->data()[idx + 2];
					//drawPixel(_j, _i, _r, _g, _b);
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

