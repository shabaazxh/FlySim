#include <iostream>
#include "Cartesian3.h"
#include "Matrix4.h"
class Quaternion
{
public:
    float w, x, y, z;

    Quaternion(float angle, const Cartesian3& v);
    Quaternion(float w, float x, float y, float z);
    columnMajorMatrix ToRotationMatrix();
    void Normalize();
    Quaternion Conjugate();
};

inline Quaternion operator*(const Quaternion& left, const Quaternion& right)
{
    float w = (left.w * right.w) - (left.x * right.x) - (left.y * right.y) - (left.z * right.z);
    float x = (left.x * right.w) + (left.w * right.x) + (left.y * right.z) - (left.z * right.y);
    float y = (left.y * right.w) + (left.w * right.y) + (left.z * right.x) - (left.x * right.z);
    float z = (left.z * right.w) + (left.w * right.z) + (left.x * right.y) - (left.y * right.x);

    return {w, x, y, z};
}

inline Quaternion operator*(const Quaternion& rot, const Cartesian3& v)
{
    float w = - (rot.x * v.x) - (rot.y * v.y) - (rot.z * v.z);
    float x =   (rot.w * v.x) + (rot.y * v.z) - (rot.z * v.y);
    float y =   (rot.w * v.y) + (rot.z * v.x) - (rot.x * v.z);
    float z =   (rot.w * v.z) + (rot.x * v.y) - (rot.y * v.x);

    return {w, x, y, z};
}
