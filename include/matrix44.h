#pragma once

#include <quaternion.h>
#include <vector3.h>

class Matrix44 {
private:
    float _data[16];
public:        
    Matrix44& setRotation(const Quaternion& rotation);
    Matrix44& scale(const Vector3& _scale);
    Matrix44& setPosition(const Vector3& position);
    Matrix44& compose(const Vector3& position, const Quaternion& rotation, const Vector3& scale);
    
    static Matrix44 makePerspective(float fovRadians, float aspectRatio, float zNear, float zFar);

    Matrix44 operator * (const Matrix44& other) const;
    Vector3 operator * (const Vector3& v) const;

    bool getInverse(Matrix44& out) const;
    Matrix44& setIdentity();
};
