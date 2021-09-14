
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

	Vector3 Transform::worldForward() const {
		Vector3 dummy;
		Quaternion worldRotation;
		getWorldMatrix().decompose(dummy, worldRotation, dummy);
		return (worldRotation * Vector3::forward).normalized();
	}

	Vector3 Transform::worldRight() const {
		Vector3 dummy;
		Quaternion worldRotation;
		getWorldMatrix().decompose(dummy, worldRotation, dummy);
		return (worldRotation * Vector3::right).normalized();
	}

	Vector3 Transform::worldUp() const {
		Vector3 dummy;
		Quaternion worldRotation;
		getWorldMatrix().decompose(dummy, worldRotation, dummy);
		return (worldRotation * Vector3::up).normalized();
	}
}

