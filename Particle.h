#include <iostream>
#include "Matrix4.h"
class Particle
{
public:
    Cartesian3 position;
    Cartesian3 velocity;
    float life, size, mass; // size is scale()
    //-9.81
    float gravity = -19.81f;
    Cartesian3 direction;
    float altitude;

    float radius = 0.0f;
    float collisionSphereRadius = 86.0f;
    GLfloat lavaBombColour[4] = {0.5, 0.3, 0.0, 1.0};

    columnMajorMatrix world;
    columnMajorMatrix view;

    Particle() {}
    Particle(const char *fileName, const Cartesian3& direction, float speed, float r)
    {
        lavaBombModel.ReadFileTriangleSoup(fileName);
        Sphere.ReadFileTriangleSoup(fileName);

        position = Cartesian3(-38500.0f, 1000.0f, -4000);
        this->direction = direction;
        velocity.x = direction.x;
        velocity.y = direction.y;
        velocity.z = direction.z;
        life = 10000.0f;
        mass = 1.0f; //100 kg
        altitude = 100.0f;
        radius = r;
    }

    void Push(const Cartesian3& pushAmount)
    {
        Cartesian3 acceleration = pushAmount / mass;
        
        velocity = velocity + acceleration;
    }
    
    void RenderChildren(columnMajorMatrix& WorldMatrix, columnMajorMatrix& viewMatrix)
    {   
        float size = 0.5f;
        auto mat = viewMatrix * (modelMatrix * WorldMatrix) * columnMajorMatrix::Scale(Cartesian3(size, size, size));	

        for(int i = 0; i < 5; i++)
        {
            lavaBombModel.Render(mat);
        }
    }

    bool isColliding(const Particle& other) const
    {
        float d = (position - other.position).length();

        return d < (collisionSphereRadius + other.collisionSphereRadius);
    }
    
    bool isCollidingWithFloor(float height)
    {   
        return position.y - radius <= height;
    }

    void update(float dt, const columnMajorMatrix& worldMatrix, const columnMajorMatrix& viewMatrix)
    {
        //Cartesian3 acc = gravity / mass;
        //velocity = velocity + acc * dt * dt / 2;
        
        //velocity.y += gravity * dt;

        world = worldMatrix;
        view = viewMatrix;

        // Update position (s = ut + 1/2at^2)
        position.x += velocity.x * dt;
        position.y += velocity.y * dt - 0.5 * gravity * dt * dt;
        position.z += velocity.z * dt;

        // Update velocity (v = u + at)
        velocity.x = velocity.x;
        velocity.y = velocity.y + gravity * dt;
        velocity.z = velocity.z;

        if(position.y <= 0)
        {
            position.y = 0;
            velocity.y = 0;
            life = 0.0f;
        }

        //std::cout << "Altitude: " << position.y << std::endl;
        // make world matrix extern or something to fix order of operation?
        modelMatrix = view * columnMajorMatrix::Translate(position) * world * columnMajorMatrix::Scale(Cartesian3(radius, radius, radius));
    }

    HomogeneousFaceSurface lavaBombModel;
    HomogeneousFaceSurface Sphere;
    columnMajorMatrix modelMatrix;
};