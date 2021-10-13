
#include "pch.h"
#include "transform.h"

namespace platz {
	DEFINE_OBJECT(Transform);

	const Matrix44& Transform::worldMatrix() {
		if (_worldMatrixDirty) {
			_worldMatrix = zmath::Matrix44::compose(_position, _rotation, _scale);
			_worldMatrixDirty = false;
		}
		return _worldMatrix;
	}

	Vector3 Transform::worldForward() {
		Vector3 dummy;
		Quaternion worldRotation;
		worldMatrix().decompose(dummy, worldRotation, dummy);
		return (worldRotation * Vector3::forward).normalized();
	}

	Vector3 Transform::worldRight() {
		Vector3 dummy;
		Quaternion worldRotation;
		worldMatrix().decompose(dummy, worldRotation, dummy);
		return (worldRotation * Vector3::right).normalized();
	}

	Vector3 Transform::worldUp() {
		Vector3 dummy;
		Quaternion worldRotation;
		worldMatrix().decompose(dummy, worldRotation, dummy);
		return (worldRotation * Vector3::up).normalized();
	}

	void Transform::preUpdate() {
		_worldMatrixDirty = true;
	}
}

