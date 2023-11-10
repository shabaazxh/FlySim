#include "Plane.h"

Plane::Plane(const char *fileName, const Cartesian3& startPosition, float collisionRadius, const PlaneRole& role)
{
    planeModel.ReadFileTriangleSoup(fileName);
    // This is for debug to see size of the collision "sphere"
    Sphere.ReadFileTriangleSoup("./models/lavaBombModel.tri");

    position = startPosition;
    forward = Cartesian3(-1, 0, 0);
    up = Cartesian3(0, 1, 0);
    collisionSphereRadius = collisionRadius;
    m_planeRole = role;
}

bool Plane::isColliding(const Plane& other) 
{
    float planeCollision = (position - other.position).length();
    return planeCollision < (collisionSphereRadius + other.collisionSphereRadius);
}

bool Plane::isCollidingWithFloor(float height)
{   
    return position.y - collisionSphereRadius <= height;
}

bool Plane::isCollidingWithParticle(const Particle& particle)
{
    float distance = (position - particle.GetPosition()).length();
    return distance < (collisionSphereRadius + particle.GetCollisionSphereRadius());
}

void Plane::Update(float dt, const columnMajorMatrix& worldMatrix, const columnMajorMatrix& viewMatrix)
{
    deltaTime = dt;
    if(m_planeRole == PlaneRole::Particle)
    {
        Cartesian3 circleCenter = Cartesian3(0, position.y, 0);
    
        float x = flightPathRadius * cos(angle);
        float y = flightPathRadius * sin(angle);

        auto angularSpeed = speed / flightPathRadius;

        angle += angularSpeed * dt;

        if(angle > 2 * M_PI)
        {
            angle -= 2 * M_PI;
        }

        if(angle < 0)
        {
            angle += 2 * M_PI;
        }
        previousPosition = position;
        position = circleCenter + Cartesian3(x, 0.0f, y);    
        direction = position - previousPosition;
        direction = direction.unit();

        auto rotationMatrix = columnMajorMatrix::Look(position, position + direction, Cartesian3(0, 1, 0));

        // Construct model matrix to apply transformations to the object
        // Increased scale of the AI flying planes since it's incredibly difficult to see them with scale 1
        modelMatrix = viewMatrix * columnMajorMatrix::Translate(position) * rotationMatrix * 
        worldMatrix * 
        // size increase to see plane: realistic plane size of A320 is about Length: 37 meters, Wingspan 36 meters, Height 12 meters but these values 
        // are too small to see in our game so I have exaggerated the size to make it somewhat visible 
        columnMajorMatrix::Scale(Cartesian3(scale, scale, scale));
        //columnMajorMatrix::Scale(Cartesian3(1, -1, 1))

        sphereMatrix = viewMatrix * columnMajorMatrix::Translate(position) * worldMatrix * columnMajorMatrix::Scale(Cartesian3(4.0f, 4.0f, 4.0f));
    } else 
    {
        if(pitch >= 89.0f) pitch = 89.0f;
        if(pitch <= -89.0f) pitch = -89.0f;

        // Use forward to rotate forward with yaw
        auto up = Cartesian3(0, 1, 0);
        auto forward = Cartesian3(0,0,-1);
        
        // Apply yaw rotation
        forward.Rotate(yaw, up);
        forward = forward.unit();

        // Calculate x direction and rotate forward using pitch
        Cartesian3 x = up;
        x = x.cross(forward);
        x = x.unit();
        forward.Rotate(pitch, x);
        forward = forward.unit();
        
        // Find new up
        Cartesian3 u = forward.cross(x);
        u = u.unit();

        u.Rotate(roll, forward);
        u = u.unit();

        // Set camera direction and up
        direction = forward;
        up = u;

        // direction.x = std::cos(DEG2RAD(yaw)) * std::cos(DEG2RAD(pitch));
        // direction.y = std::sin(DEG2RAD(pitch));
        // direction.z = std::sin(DEG2RAD(yaw)) * std::cos(DEG2RAD(pitch));
        // direction = direction.unit();

        // auto right = Cartesian3(0.0f, 1.0f, 0.0f).cross(direction);
        // right = right.unit();

        // up = direction.cross(right);
        // up = up.unit();

        // if(roll != 0)
        // {
        //     auto rollMatrix = columnMajorMatrix::Rotate(roll,
        //     direction);

        //     auto tempUp = rollMatrix * up;
        //     auto tempRight = rollMatrix * right;

        //     up = Cartesian3(tempUp.x, tempUp.y, tempUp.z);
        //     right = Cartesian3(tempRight.x, tempRight.y, tempRight.z);
        // }

        auto look = columnMajorMatrix::Look(position, position + direction, up);
        modelMatrix = look;
    }

}
void Plane::SetScale(float s)
{
    scale = s;
}
// Increase the speed of the plane
void Plane::IncreaseSpeed()
{
    movementSpeed += 10.0f;
    //movementSpeed = std::min(movementSpeed + 1.0f, 9.0f);
    std::cout << movementSpeed << std::endl;
}
// Decrease the speed of the plane
void Plane::DecreaseSpeed()
{   
    movementSpeed = std::max(movementSpeed - 1.0f, 0.0f);
    std::cout << movementSpeed << std::endl;
}

// Controls for the plane
void Plane::Forward()
{
    position = position + movementSpeed * direction;
}

void Plane::Back()
{
    position = position - movementSpeed * direction;
}

void Plane::Right()
{
    yaw -= turnSpeed * deltaTime;
}

void Plane::Left()
{
    yaw += turnSpeed * deltaTime;
}

void Plane::PitchUp()
{
    pitch += turnSpeed * deltaTime;
}

void Plane::PitchDown()
{
    pitch -= turnSpeed * deltaTime;
}

void Plane::RollRight()
{
    roll += turnSpeed * deltaTime;
}

void Plane::RollLeft()
{
    roll -= turnSpeed * deltaTime;
}