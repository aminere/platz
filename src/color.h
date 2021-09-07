#pragma once

namespace platz {
	class Color {
	public:

		static Color black;
		static Color white;
		static Color red;
		static Color green;
		static Color blue;

		float r = 0.f;
		float g = 0.f;
		float b = 0.f;
		float a = 1.f;

		Color() = default;
		Color(float _r, float _g, float _b, float _a)
			: r(_r)
			, g(_g)
			, b(_b)
			, a(_a) {

		}
	};
}

