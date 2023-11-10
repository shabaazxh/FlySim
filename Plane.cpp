#include "Plane.h"

Plane::Plane(const char *fileName, const Cartesian3& startPosition, float collisionRadius, bool clockwise, const PlaneRole& role)
{
    planeModel.ReadFileTriangleSoup(fileName);

    m_position = startPosition;
    m_forward = Cartesian3(-1, 0, 0);
    m_up = Cartesian3(0, 1, 0);
    m_collisionSphereRadius = collisionRadius;
    m_planeRole = role;
    m_clockWise = clockwise;
}

bool Plane::isColliding(const Plane& other) 
{
    float planeCollision = (m_position - other.GetPostion()).length();
    return planeCollision < (m_collisionSphereRadius + other.m_collisionSphereRadius);
}

bool Plane::isCollidingWithFloor(float height)
{   
    return m_position.y - m_collisionSphereRadius <= height;
}

bool Plane::isCollidingWithParticle(const Particle& particle)
{
    float distance = (m_position - particle.GetPosition()).length();
    return distance < (m_collisionSphereRadius + particle.GetCollisionSphereRadius());
}

void Plane::Update(float dt, const columnMajorMatrix& worldMatrix, const columnMajorMatrix& viewMatrix)
{
    deltaTime = dt;
    if(m_planeRole == PlaneRole::Particle)
    {
        Cartesian3 circleCenter = Cartesian3(0, m_position.y, 0);

        // Take cos and sin to get x and z for the flight path 
        float x = m_flightPathRadius * cos(m_angle);
        float z = m_flightPathRadius * sin(m_angle);

        // auto angularSpeed = speed / flightPathRadius; // to correctly move the plane around using it's speed and radius of circle
        auto angularSpeed = 0.2f; // setting it to a small value so  the planes move slower. This will allow plane collision to be seen
        m_angle += (m_clockWise ? -1 : 1) * angularSpeed * dt; //

        // Ensure angles being used for circular path remain within 0 - 2pi radians
        if(m_angle > 2 * M_PI)
        {
            m_angle -= 2 * M_PI;
        }

        if(m_angle < 0)
        {
            m_angle += 2 * M_PI;
        }
        // Calculate the next m_direction based on the previous position and current position
        m_previousPosition = m_position;
        m_position = circleCenter + Cartesian3(x, 0.0f, z);    
        m_direction = m_position - m_previousPosition;
        m_direction = m_direction.unit();
        // Construct the rotation look matrix to ensure the plane looks in the correct m_direction
        // when flying around the circular flight path
        auto rotationMatrix = columnMajorMatrix::Look(m_position, m_position + m_direction, Cartesian3(0, 1, 0));

        // Construct model matrix to apply transformations to the object
        // Increased m_scale of the AI flying planes since it's incredibly difficult to see them with m_scale 1
        modelMatrix = viewMatrix * columnMajorMatrix::Translate(m_position) * rotationMatrix * 
        worldMatrix * 
        // size increase to see plane: realistic plane size of A320 is about Length: 37 meters, Wingspan 36 meters, Height 12 meters but these values 
        // are too small to see in our game so I have exaggerated the size to make it somewhat visible 
        columnMajorMatrix::Scale(Cartesian3(m_scale, m_scale, m_scale));
    } else 
    {
        if(m_pitch >= 89.0f) m_pitch = 89.0f;
        if(m_pitch <= -89.0f) m_pitch = -89.0f;

        // Use forward to rotate forward with m_yaw
        auto up = Cartesian3(0, 1, 0);
        auto forward = Cartesian3(0,0,-1);
        
        // Apply m_yaw rotation
        forward.Rotate(m_yaw, up);
        forward = forward.unit();

        // Calculate x m_direction and rotate forward using m_pitch
        Cartesian3 x = up;
        x = x.cross(forward);
        x = x.unit();
        forward.Rotate(m_pitch, x);
        forward = forward.unit();
        
        // Find new up
        Cartesian3 u = forward.cross(x);
        u = u.unit();

        u.Rotate(m_roll, forward);
        u = u.unit();

        // Set camera m_direction and up
        m_direction = forward;
        up = u;

        // construct look matrix using new m_direction 
        auto look = columnMajorMatrix::Look(m_position, m_position + m_direction, up);
        modelMatrix = viewMatrix * columnMajorMatrix::Translate(m_position) * look * worldMatrix * columnMajorMatrix::Scale(Cartesian3(m_scale,m_scale,m_scale));
    }

}

void Plane::SetColor(float r, float g, float b, float a)
{
    planeColour[0] = r;
    planeColour[1] = g;
    planeColour[2] = b;
    planeColour[3] = a;
}
void Plane::SetScale(float s)
{
    m_scale = s;
}
// Increase the speed of the plane
void Plane::IncreaseSpeed()
{
    m_movementSpeed += 10.0f;
    //m_movementSpeed = std::min(m_movementSpeed + 1.0f, 9.0f);
    std::cout << m_movementSpeed << std::endl;
}
// Decrease the speed of the plane
void Plane::DecreaseSpeed()
{   
    m_movementSpeed = std::max(m_movementSpeed - 1.0f, 0.0f);
    std::cout << m_movementSpeed << std::endl;
}

// Controls for the plane
void Plane::Forward()
{
    m_position = m_position + m_movementSpeed * m_direction;
}

void Plane::Back()
{
    m_position = m_position - m_movementSpeed * m_direction;
}

void Plane::Right()
{
    m_yaw -= m_turnSpeed * deltaTime;
}

void Plane::Left()
{
    m_yaw += m_turnSpeed * deltaTime;
}

void Plane::PitchUp()
{
    m_pitch += m_turnSpeed * deltaTime;
}

void Plane::PitchDown()
{
    m_pitch -= m_turnSpeed * deltaTime;
}

void Plane::RollRight()
{
    m_roll += m_turnSpeed * deltaTime;
}

void Plane::RollLeft()
{
    m_roll -= m_turnSpeed * deltaTime;
}