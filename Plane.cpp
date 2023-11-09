#include "Plane.h"

void Plane::IncreaseSpeed(float speed)
{
    movementSpeed += speed;
    std::cout << movementSpeed << std::endl;
}

void Plane::DecreaseSpeed(float speed)
{
    movementSpeed -= speed;
    std::cout << movementSpeed << std::endl;
}