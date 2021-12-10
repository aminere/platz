#pragma once

namespace platz {
	class Color {
	public:

		static Color black;
		static Color white;
		static Color red;
		static Color green;
		static Color blue;

		float r = 1.f;
		float g = 1.f;
		float b = 1.f;
		float a = 1.f;

		Color() = default;
		Color(float _r, float _g, float _b, float _a = 1.f)
			: r(std::min(std::max(_r, 0.f), 1.f))
			, g(std::min(std::max(_g, 0.f), 1.f))
			, b(std::min(std::max(_b, 0.f), 1.f))
			, a(_a) {

		}

		inline Color operator * (float f) const {
			return Color(r * f, g * f, b * f, a * f);
		}

		inline Color operator * (const Color& other) const {
			return Color(r * other.r, g * other.g, b * other.b, a * other.a);
		}

		inline Color operator + (const Color& other) const {
			return Color(r + other.r, g + other.g, b + other.b, a + other.a);
		}
	};
}

