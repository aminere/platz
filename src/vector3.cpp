
#include <vector3.h>

Vector3 Vector3::create(float x, float y, float z) {
	Vector3 v;
	return v.set(x, y, z);
}

Vector3 Vector3::zero = Vector3::create(0.f, 0.f, 0.f);
Vector3 Vector3::one = Vector3::create(1.f, 1.f, 1.f);
