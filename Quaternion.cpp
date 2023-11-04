#include "Quaternion.h"


Quaternion::Quaternion(float angle, const Cartesian3& v)
{
    float angleRadians = angle * M_PI / 180;
    float rotAngle = angleRadians / 2;

    // to degrees = radians * (180 / M_PI);
    float cosAngle = std::cos(rotAngle);
    float sinAngle = std::sin(rotAngle);

    w = cosAngle;
    x = v.x * sinAngle;
    y = v.y * sinAngle;
    z = v.z * sinAngle;
}


Quaternion::Quaternion(float w, float x, float y, float z)
{
    this->w = w;
    this->x = x;
    this->y = y;
    this->z = z;
}

columnMajorMatrix Quaternion::ToRotationMatrix()
{
    float x2 = x * x;
    float y2 = y * y;
    float z2 = z * z;
    float xy = x * y;
    float xz = x * z;
    float yz = y * z;
    float wx = w * x;
    float wy = w * y;
    float wz = w * z;

    columnMajorMatrix ret;

    ret.coordinates[0] = 1.0f - 2.0f * (y2 + z2);
    ret.coordinates[1] = 2.0f * (xy - wz);
    ret.coordinates[2] = 2.0f * (xz + wy);
    ret.coordinates[3] = 0.0f;

    ret.coordinates[4] = 2.0f * (xy + wz);
    ret.coordinates[5] = 1.0f - 2.0f * (x2 + z2);
    ret.coordinates[6] = 2.0f * (yz + wx);
    ret.coordinates[7] = 0.0f;

    ret.coordinates[8] = 2.0f * (xz + wy);
    ret.coordinates[9] = 2.0f * (yz - wx);
    ret.coordinates[10] = 1.0f - 2.0f * (x2 + y2);
    ret.coordinates[11] = 0.0f;

    ret.coordinates[12] = 0.0f;
    ret.coordinates[13] = 0.0f;
    ret.coordinates[14] = 0.0f;
    ret.coordinates[15] = 1.0f;

    return ret;
}

void Quaternion::Normalize()
{
    float magnitude = std::sqrtf(w*w + x*x + y*y + z*z);

    w /= magnitude;
    x /= magnitude;
    y /= magnitude;
    z /= magnitude;
}

Quaternion Quaternion::Conjugate()
{
    return {w, -x, -y, -z};
}
