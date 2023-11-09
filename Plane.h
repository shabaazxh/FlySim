#include <iostream>
#include "Matrix4.h"

enum class PlaneRole
{
    Particle,
    Controller
};
class Plane
{
public:
    Plane(const char *fileName, const Cartesian3& startPosition, const Cartesian3& dir, float rotAngle,
    float collisionRadius, const PlaneRole& role)
    {
        planeModel.ReadFileTriangleSoup(fileName);
        Sphere.ReadFileTriangleSoup("./models/lavaBombModel.tri");
        position = startPosition;
        direction = dir;
        forward = Cartesian3(-1, 0, 0);
        up = Cartesian3(0, 1, 0);
        dirAngle = rotAngle;
        collisionSphereRadius = collisionRadius;
        m_planeRole = role;
    }

    bool isColliding(const Plane& other) const
    {   
        float planeCollision = (position - other.position).length();
        return planeCollision < (collisionSphereRadius + other.collisionSphereRadius);
    }

    bool isCollidingWithParticle(const Particle& particle)
    {
        float distance = (position - particle.GetPosition()).length();
        return distance < (collisionSphereRadius + particle.GetCollisionSphereRadius());
    }
    
    bool isCollidingWithFloor(float height)
    {   
        //std::cout << "height: " << height << " pos: " << position.y - collisionSphereRadius << std::endl;
        return position.y - collisionSphereRadius <= height;
    }

    void update(float dt, const columnMajorMatrix& worldMatrix, const columnMajorMatrix& viewMatrix)
    {
        deltaTime = dt;
        if(m_planeRole == PlaneRole::Particle)
        {
            Cartesian3 circleCenter = Cartesian3(0, position.y, 0);
        
            float x = radius * cos(angle);
            float y = radius * sin(angle);

            auto angularSpeed = speed / radius;

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

            // were in world space so we scale, rotate then translate
            modelMatrix = viewMatrix * columnMajorMatrix::Translate(position) * rotationMatrix * 
            columnMajorMatrix::Scale(Cartesian3(1, -1, 1)) * worldMatrix * 
            columnMajorMatrix::Scale(Cartesian3(scale, scale, scale));

            sphereMatrix = viewMatrix * columnMajorMatrix::Translate(position) * worldMatrix;
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

    // Controls
    void Forward()
    {
        //std::cout << deltaTime << std::endl;
        position = position + 10000.0f * direction * deltaTime;
    }

    void Back()
    {
        position = position - 500.0f * direction;
    }

    void Right()
    {
        yaw -= 6.0f;
    }

    void Left()
    {
        yaw += 6.0f;
    }

    void PitchUp()
    {
        pitch += 6.0f;
    }

    void PitchDown()
    {
        pitch -= 6.0f;
    }

    void RollRight()
    {
        roll += 6.0f;
    }

    void RollLeft()
    {
        roll -= 6.0f;
    }

    HomogeneousFaceSurface planeModel;
    HomogeneousFaceSurface Sphere;
    columnMajorMatrix modelMatrix;
    columnMajorMatrix sphereMatrix;

    Cartesian3 position;
    Cartesian3 previousPosition;
    Cartesian3 direction;
    Cartesian3 up;
    float dirAngle = 90.0f;

    float scale = 1.0f; // 73
    float radius = 3000.0f;
    float angle = 0.0f;
    float speed = 900.0f;

    float pitch = 0.0f;
    float yaw = 0.0f;
    float roll = 0.0f;

    float collisionSphereRadius = 86.0f;
    float collisionSphereRadiusPlayer = 200.0f;
    Cartesian3 forward;
    GLfloat lavaBombColour[4] = {0.5, 0.3, 0.0, 1.0};
    float offset = 0.0f;
    bool shouldRender = true;

    PlaneRole m_planeRole;
    float deltaTime;
};