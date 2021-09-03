
#include <matrix44.h>
#include <math.h>

Matrix44& Matrix44::setRotation(const Quaternion& rotation) {
    // based on Threejs
    const auto x = rotation.x(), y = rotation.y(), z = rotation.z(), w = rotation.w();
    const auto x2 = x + x, y2 = y + y, z2 = z + z;
    const auto xx = x * x2, xy = x * y2, xz = x * z2;
    const auto yy = y * y2, yz = y * z2, zz = z * z2;
    const auto wx = w * x2, wy = w * y2, wz = w * z2;

    _data[0] = 1.f - (yy + zz);
    _data[4] = xy - wz;
    _data[8] = xz + wy;

    _data[1] = xy + wz;
    _data[5] = 1 - (xx + zz);
    _data[9] = yz - wx;

    _data[2] = xz - wy;
    _data[6] = yz + wx;
    _data[10] = 1.f - (xx + yy);

    _data[3] = 0;
    _data[7] = 0;
    _data[11] = 0;

    _data[12] = 0;
    _data[13] = 0;
    _data[14] = 0;
    _data[15] = 1;

    return *this;
}

Matrix44& Matrix44::scale(const Vector3& scale) {
    const auto x = scale.x(), y = scale.y(), z = scale.z();
    _data[0] *= x; _data[4] *= y; _data[8] *= z;
    _data[1] *= x; _data[5] *= y; _data[9] *= z;
    _data[2] *= x; _data[6] *= y; _data[10] *= z;
    _data[3] *= x; _data[7] *= y; _data[11] *= z;
    return *this;
}

Matrix44& Matrix44::setPosition(const Vector3& position) {    
    _data[12] = position.x();
    _data[13] = position.y();
    _data[14] = position.z();
    return *this;
}

Matrix44& Matrix44::compose(const Vector3& position, const Quaternion& rotation, const Vector3& _scale) {
    setRotation(rotation);
    scale(_scale);
    setPosition(position);
    return *this;
}

Matrix44 Matrix44::operator*(const Matrix44& other) const {
    // based on Threejs
    Matrix44 out;
    const auto ae = _data;
    const auto be = other._data;
    const auto te = out._data;

    const auto a11 = ae[0], a12 = ae[4], a13 = ae[8], a14 = ae[12];
    const auto a21 = ae[1], a22 = ae[5], a23 = ae[9], a24 = ae[13];
    const auto a31 = ae[2], a32 = ae[6], a33 = ae[10], a34 = ae[14];
    const auto a41 = ae[3], a42 = ae[7], a43 = ae[11], a44 = ae[15];

    const auto b11 = be[0], b12 = be[4], b13 = be[8], b14 = be[12];
    const auto b21 = be[1], b22 = be[5], b23 = be[9], b24 = be[13];
    const auto b31 = be[2], b32 = be[6], b33 = be[10], b34 = be[14];
    const auto b41 = be[3], b42 = be[7], b43 = be[11], b44 = be[15];

    te[0] = a11 * b11 + a12 * b21 + a13 * b31 + a14 * b41;
    te[4] = a11 * b12 + a12 * b22 + a13 * b32 + a14 * b42;
    te[8] = a11 * b13 + a12 * b23 + a13 * b33 + a14 * b43;
    te[12] = a11 * b14 + a12 * b24 + a13 * b34 + a14 * b44;

    te[1] = a21 * b11 + a22 * b21 + a23 * b31 + a24 * b41;
    te[5] = a21 * b12 + a22 * b22 + a23 * b32 + a24 * b42;
    te[9] = a21 * b13 + a22 * b23 + a23 * b33 + a24 * b43;
    te[13] = a21 * b14 + a22 * b24 + a23 * b34 + a24 * b44;

    te[2] = a31 * b11 + a32 * b21 + a33 * b31 + a34 * b41;
    te[6] = a31 * b12 + a32 * b22 + a33 * b32 + a34 * b42;
    te[10] = a31 * b13 + a32 * b23 + a33 * b33 + a34 * b43;
    te[14] = a31 * b14 + a32 * b24 + a33 * b34 + a34 * b44;

    te[3] = a41 * b11 + a42 * b21 + a43 * b31 + a44 * b41;
    te[7] = a41 * b12 + a42 * b22 + a43 * b32 + a44 * b42;
    te[11] = a41 * b13 + a42 * b23 + a43 * b33 + a44 * b43;
    te[15] = a41 * b14 + a42 * b24 + a43 * b34 + a44 * b44;
    return out;
}

Vector3 Matrix44::operator*(const Vector3& v) const {    
    const auto x = v.x(), y = v.y(), z = v.z();
    const auto result = Vector3::create(
        _data[0] * x + _data[4] * y + _data[8] * z + _data[12],
        _data[1] * x + _data[5] * y + _data[9] * z + _data[13],
        _data[2] * x + _data[6] * y + _data[10] * z + _data[14]
    );
    const auto w = _data[3] * x + _data[7] * y + _data[11] * z + _data[15];
    return result / w;
}

bool Matrix44::getInverse(Matrix44& out) const {
    // based on Threejs
    const auto te = out._data;
    const auto me = _data;
    const auto n11 = me[0], n21 = me[1], n31 = me[2], n41 = me[3],
        n12 = me[4], n22 = me[5], n32 = me[6], n42 = me[7],
        n13 = me[8], n23 = me[9], n33 = me[10], n43 = me[11],
        n14 = me[12], n24 = me[13], n34 = me[14], n44 = me[15],
        t11 = n23 * n34 * n42 - n24 * n33 * n42 + n24 * n32 * n43 - n22 * n34 * n43 - n23 * n32 * n44 + n22 * n33 * n44,
        t12 = n14 * n33 * n42 - n13 * n34 * n42 - n14 * n32 * n43 + n12 * n34 * n43 + n13 * n32 * n44 - n12 * n33 * n44,
        t13 = n13 * n24 * n42 - n14 * n23 * n42 + n14 * n22 * n43 - n12 * n24 * n43 - n13 * n22 * n44 + n12 * n23 * n44,
        t14 = n14 * n23 * n32 - n13 * n24 * n32 - n14 * n22 * n33 + n12 * n24 * n33 + n13 * n22 * n34 - n12 * n23 * n34;

    const auto det = n11 * t11 + n21 * t12 + n31 * t13 + n41 * t14;
    if (det == 0.f) {
        return false;
    }

    const auto detInv = 1.f / det;
    te[0] = t11 * detInv;
    te[1] = (n24 * n33 * n41 - n23 * n34 * n41 - n24 * n31 * n43 + n21 * n34 * n43 + n23 * n31 * n44 - n21 * n33 * n44) * detInv;
    te[2] = (n22 * n34 * n41 - n24 * n32 * n41 + n24 * n31 * n42 - n21 * n34 * n42 - n22 * n31 * n44 + n21 * n32 * n44) * detInv;
    te[3] = (n23 * n32 * n41 - n22 * n33 * n41 - n23 * n31 * n42 + n21 * n33 * n42 + n22 * n31 * n43 - n21 * n32 * n43) * detInv;

    te[4] = t12 * detInv;
    te[5] = (n13 * n34 * n41 - n14 * n33 * n41 + n14 * n31 * n43 - n11 * n34 * n43 - n13 * n31 * n44 + n11 * n33 * n44) * detInv;
    te[6] = (n14 * n32 * n41 - n12 * n34 * n41 - n14 * n31 * n42 + n11 * n34 * n42 + n12 * n31 * n44 - n11 * n32 * n44) * detInv;
    te[7] = (n12 * n33 * n41 - n13 * n32 * n41 + n13 * n31 * n42 - n11 * n33 * n42 - n12 * n31 * n43 + n11 * n32 * n43) * detInv;

    te[8] = t13 * detInv;
    te[9] = (n14 * n23 * n41 - n13 * n24 * n41 - n14 * n21 * n43 + n11 * n24 * n43 + n13 * n21 * n44 - n11 * n23 * n44) * detInv;
    te[10] = (n12 * n24 * n41 - n14 * n22 * n41 + n14 * n21 * n42 - n11 * n24 * n42 - n12 * n21 * n44 + n11 * n22 * n44) * detInv;
    te[11] = (n13 * n22 * n41 - n12 * n23 * n41 - n13 * n21 * n42 + n11 * n23 * n42 + n12 * n21 * n43 - n11 * n22 * n43) * detInv;

    te[12] = t14 * detInv;
    te[13] = (n13 * n24 * n31 - n14 * n23 * n31 + n14 * n21 * n33 - n11 * n24 * n33 - n13 * n21 * n34 + n11 * n23 * n34) * detInv;
    te[14] = (n14 * n22 * n31 - n12 * n24 * n31 - n14 * n21 * n32 + n11 * n24 * n32 + n12 * n21 * n34 - n11 * n22 * n34) * detInv;
    te[15] = (n12 * n23 * n31 - n13 * n22 * n31 + n13 * n21 * n32 - n11 * n23 * n32 - n12 * n21 * n33 + n11 * n22 * n33) * detInv;
    return true;
}

Matrix44& Matrix44::setIdentity() {
    const auto m = _data;
    m[0] = 1; m[4] = 0; m[8] = 0; m[12] = 0;
    m[1] = 0; m[5] = 1; m[9] = 0; m[13] = 0;
    m[2] = 0; m[6] = 0; m[10] = 1; m[14] = 0;
    m[3] = 0; m[7] = 0; m[11] = 0; m[15] = 1;
    return *this;
}

Matrix44 Matrix44::makePerspective(float fovRadians, float aspectRatio, float zNear, float zFar) {
    // based on https://www.gamedev.net/articles/programming/graphics/perspective-projections-in-lh-and-rh-systems-r3598/
    Matrix44 out;
    const auto m = out._data;
    const auto f = 1.f / (float)tan(fovRadians / 2.f);
    const auto invRange = 1.f / (zNear - zFar);
    const auto a = (zFar + zNear) * invRange;
    // LH projection
    // const invRange = 1 / (zFar - zNear);
    // const a = -(zFar + zNear) * invRange;
    const auto b = (zNear * zFar * 2.f) * invRange;
    m[0] = f / aspectRatio; m[1] = 0.0;  m[2] = 0.0; m[3] = 0.0;
    m[4] = 0.0;             m[5] = f;    m[6] = 0.0; m[7] = 0.0;
    m[8] = 0.0;             m[9] = 0.0;  m[10] = a;  m[11] = -1; // flip for LH projection
    m[12] = 0.0;            m[13] = 0.0; m[14] = b;  m[15] = 0.0;
    return out;
}

