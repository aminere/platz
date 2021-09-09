#pragma once

#include "component.h"

#include "vector3.h"
#include "quaternion.h"
#include "matrix44.h"

namespace platz {

	using namespace zmath;

	class Transform : public Component {

		DECLARE_OBJECT(Transform, Component);

	private:

		Vector3 _position;
		Quaternion _rotation;
		Vector3 _scale;

		std::weak_ptr<Transform> _parent;

	public:

		Transform() = default;

		Transform(
			const Vector3& position,
			const Quaternion& rotation,
			const Vector3& scale
		) :
			_position(position),
			_rotation(rotation),
			_scale(scale) 	{
		}

		inline const Vector3& position() const { return _position; }
		inline void position(const Vector3& v) { _position = v; }
		inline const Quaternion& rotation() const { return _rotation; }
		inline void rotation(const Quaternion& q) { _rotation = q; }
		inline const Vector3& scale() const { return _scale; }
		inline void scale(const Vector3& v) { _scale = v; }

		inline Vector3 forward() const { return _rotation * Vector3::forward; }
		inline Vector3 right() const { return _rotation * Vector3::right; }
		inline Vector3 up() const { return _rotation * Vector3::up; }

		zmath::Matrix44 getLocalMatrix() const;
		zmath::Matrix44 getWorldMatrix() const;
	};
}
