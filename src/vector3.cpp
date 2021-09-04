
#include <vector3.h>
#include <math.h>

Vector3 Vector3::create(float x, float y, float z) {
	Vector3 v;
	return v.set(x, y, z);
}

Vector3 Vector3::zero = Vector3::create(0.f, 0.f, 0.f);
Vector3 Vector3::one = Vector3::create(1.f, 1.f, 1.f);
Vector3 Vector3::right = Vector3::create(1.f, 0.f, 0.f);
Vector3 Vector3::up = Vector3::create(0.f, 1.f, 0.f);
Vector3 Vector3::forward = Vector3::create(0.f, 0.f, 1.f);

float Vector3::length() const {
    return (float)sqrt((float)((_x * _x) + (_y * _y) + (_z * _z)));
}

Vector3& Vector3::normalize() {
    const auto len = length();
    if (len == 0.f) {
        //if (process.env.NODE_ENV == = "development") {
        //    console.assert(false, "Normalizing a zero Vector3");
        //}
        return *this;
    } else {
        *this = *this / len;
        return *this;
    }
}
