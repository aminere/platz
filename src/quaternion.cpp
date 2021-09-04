
#include <quaternion.h>
#include <math.h>

Quaternion Quaternion::identity = { 0, 0, 0, 1 };

Quaternion Quaternion::fromEulerAngles(float x, float y, float z, RotationOrder order) {
	// based on Threejs
	const auto c1 = (float)cos(x / 2.f);
	const auto c2 = (float)cos(y / 2.f);
	const auto c3 = (float)cos(z / 2.f);
	const auto s1 = (float)sin(x / 2.f);
	const auto s2 = (float)sin(y / 2.f);
	const auto s3 = (float)sin(z / 2.f);
	Quaternion q;
	if (order == RotationOrder::YXZ) {
		q._x = s1 * c2 * c3 + c1 * s2 * s3;
		q._y = c1 * s2 * c3 - s1 * c2 * s3;
		q._z = c1 * c2 * s3 - s1 * s2 * c3;
		q._w = c1 * c2 * c3 + s1 * s2 * s3;
	} else if (order == RotationOrder::ZYX) {
		q._x = s1 * c2 * c3 - c1 * s2 * s3;
		q._y = c1 * s2 * c3 + s1 * c2 * s3;
		q._z = c1 * c2 * s3 - s1 * s2 * c3;
		q._w = c1 * c2 * c3 + s1 * s2 * s3;
	} else if (order == RotationOrder::XYZ) {
		q._x = s1 * c2 * c3 + c1 * s2 * s3;
		q._y = c1 * s2 * c3 - s1 * c2 * s3;
		q._z = c1 * c2 * s3 + s1 * s2 * c3;
		q._w = c1 * c2 * c3 - s1 * s2 * s3;
	} else if (order == RotationOrder::ZXY) {
		q._x = s1 * c2 * c3 - c1 * s2 * s3;
		q._y = c1 * s2 * c3 + s1 * c2 * s3;
		q._z = c1 * c2 * s3 + s1 * s2 * c3;
		q._w = c1 * c2 * c3 - s1 * s2 * s3;
	} else if (order == RotationOrder::YZX) {
		q._x = s1 * c2 * c3 + c1 * s2 * s3;
		q._y = c1 * s2 * c3 + s1 * c2 * s3;
		q._z = c1 * c2 * s3 - s1 * s2 * c3;
		q._w = c1 * c2 * c3 - s1 * s2 * s3;
	} else if (order == RotationOrder::XZY) {
		q._x = s1 * c2 * c3 - c1 * s2 * s3;
		q._y = c1 * s2 * c3 - s1 * c2 * s3;
		q._z = c1 * c2 * s3 + s1 * s2 * c3;
		q._w = c1 * c2 * c3 + s1 * s2 * s3;
	}
	return q.normalize();
}

Quaternion& Quaternion::normalize() {
	auto l = length();
	if (l == 0.f) {
		//if (process.env.NODE_ENV == = "development") {
		//    console.assert(false, "Normalizing a zero Quaternion");
		//}
	} else {
		l = 1.f / l;
		_x *= l;
		_y *= l;
		_z *= l;
		_w *= l;
	}
	return *this;
}

float Quaternion::length() const {
	return (float)sqrt((float)((_x * _x) + (_y * _y) + (_z * _z) + (_w * _w)));
}

Vector3 Quaternion::operator * (const Vector3& v) {	
	// calculate quat * vector
	const auto ix = _w * v.x() + _y * v.z() - _z * v.y();
	const auto iy = _w * v.y() + _z * v.x() - _x * v.z();
	const auto iz = _w * v.z() + _x * v.y() - _y * v.x();
	const auto iw = -_x * v.x() - _y * v.y() - _z * v.z();

	// calculate result * inverse quat
	return Vector3::create(
		ix * _w + iw * -_x + iy * -_z - iz * -_y,
		iy * _w + iw * -_y + iz * -_x - ix * -_z,
		iz * _w + iw * -_z + ix * -_y - iy * -_x
	);
}
