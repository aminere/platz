#pragma once

#include "component.h"

#include "vector3.h"
#include "quaternion.h"

class Transform : public Component {

private:

	Vector3 _position;
	Quaternion _rotation;
	Vector3 _scale;

public:

	Transform(
		const Vector3& position,
		const Quaternion& rotation,
		const Vector3& scale
	) :
		_position(position),
		_rotation(rotation),
		_scale(scale)
	{
	}

	inline const Vector3& position() const { return _position; }
	inline void position(const Vector3& v) { _position = v; }
	inline const Quaternion& rotation() const { return _rotation; }
	inline void rotation(const Quaternion& q) { _rotation = q; }
	inline const Vector3& scale() const { return _scale; }
	inline void scale(const Vector3& v) { _scale = v; }

};

