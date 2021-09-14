
#include "pch.h"
#include "frustum.h"
#include "clipping.h"

namespace platz {

	Frustum::Frustum(
        Transform* transform,
		float nearW,
		float nearH,
		float farW,
		float farH,
		float near,
		float far
	) {
        Vector3 corners[Corner::CornerCount];

        auto fCenter = transform->worldPosition() + transform->worldForward() * -far;
        corners[Corner::FarTopLeft] = fCenter + transform->worldUp() * farH + transform->worldRight() * -farW;
        corners[Corner::FarTopRight] = fCenter + transform->worldUp() * farH + transform->worldRight() * farW;
        corners[Corner::FarBottomLeft] = fCenter + transform->worldUp() * -farH + transform->worldRight() * -farW;
        corners[Corner::FarBottomLeft] = fCenter + transform->worldUp() * -farH + transform->worldRight() * farW;
        
        auto nCenter = transform->worldPosition() + transform->worldForward() * -near;
        corners[Corner::NearTopLeft] = nCenter + transform->worldUp() * nearH + transform->worldRight() * -nearW;
        corners[Corner::NearTopRight] = nCenter + transform->worldUp() * nearH + transform->worldRight() * nearW;
        corners[Corner::NearBottomLeft] = nCenter + transform->worldUp() * -nearH + transform->worldRight() * -nearW;
        corners[Corner::NearBottomRight] = nCenter + transform->worldUp() * -nearH + transform->worldRight() * nearW;        

        _planes[Plane::Top] = zmath::Plane(corners[Corner::NearTopLeft], corners[Corner::FarTopLeft], corners[Corner::FarTopRight]);
        _planes[Plane::Bottom] = zmath::Plane(corners[Corner::NearBottomRight], corners[Corner::FarBottomRight], corners[Corner::FarBottomLeft]);
        _planes[Plane::Left] = zmath::Plane(corners[Corner::FarBottomLeft], corners[Corner::FarTopLeft], corners[Corner::NearTopLeft]);
        _planes[Plane::Right] = zmath::Plane(corners[Corner::NearBottomRight], corners[Corner::NearTopRight], corners[Corner::FarTopRight]);
        _planes[Plane::Near] = zmath::Plane(corners[Corner::NearBottomLeft], corners[Corner::NearTopLeft], corners[Corner::NearTopRight]);
        _planes[Plane::Far] = zmath::Plane(corners[Corner::FarBottomRight], corners[Corner::FarTopRight], corners[Corner::FarTopLeft]);        
	}

	void Frustum::clip(const zmath::Triangle& triangle, const zmath::Plane& plane, std::vector<zmath::Triangle>& out) const {
        std::vector<zmath::Triangle> in = { triangle };
        for (int i = 0; i < Plane::PlaneCount; ++i) {
            std::vector<zmath::Triangle> current;
            for (auto& tri : in) {
                std::vector<zmath::Triangle> clipped;
                if (Clipping::trianglePlane(tri, _planes[i], clipped)) {
                    current.insert(current.end(), clipped.begin(), clipped.end());
                }
            }
            in = current;
        }
        out.insert(out.end(), in.begin(), in.end());
	}
}

