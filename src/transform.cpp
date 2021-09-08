
#include "pch.h"
#include "transform.h"

namespace platz {
	DEFINE_OBJECT(Transform);

	zmath::Matrix44 Transform::getLocalMatrix() const {
		return zmath::Matrix44::compose(_position, _rotation, _scale);
	}

	zmath::Matrix44 Transform::getWorldMatrix() const {
		zmath::Matrix44 worldMatrix = getLocalMatrix();
		if (auto parent = _parent.lock()) {
			worldMatrix = parent->getWorldMatrix() * worldMatrix;
		}		
		return worldMatrix;
	}
}

