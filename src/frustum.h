#pragma once

#include "plane.h"
#include "triangle.h"
#include "transform.h"
#include "clipping.h"
#include <vector>

namespace platz {
	class Frustum {
	public:

		enum Corner {
			FarTopLeft,
			FarTopRight,
			FarBottomLeft,
			FarBottomRight,
			NearTopLeft,
			NearTopRight,
			NearBottomLeft,
			NearBottomRight,
			CornerCount
		};

		enum Plane {
			Near,
			Far,
			Left,
			Right,
			Bottom,
			Top,
			PlaneCount
		};

		Frustum(
			Transform* transform,
			float nearW,
			float nearH,
			float farW,
			float farH,
			float near,
			float far			
		);

		Clipping::Status clip(const zmath::Triangle& triangle, std::vector<zmath::Clipping::ClippedTriangle>& out) const;

	private:

		zmath::Plane _planes[Plane::PlaneCount];
	};
}

