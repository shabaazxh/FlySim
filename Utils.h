
#include <cmath>

namespace Math
{
    float ToRadians(float degrees)
    {
        return degrees * M_PI / 180;
    }

    float ToDegree(float radians)
    {
        return radians * (180 / M_PI);
    }
};