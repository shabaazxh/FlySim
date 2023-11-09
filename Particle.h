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
    GLfloat childSmoke[4] = {1.0f, 1.0f, 1.0f, 1.0};

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
    // Destructor to clean up heap allocated resources within the class
    ~Particle()
    {
        for(auto& child : children)
        {
            if(child != nullptr)
            {
                delete child;
            }
        }
    }

    void CreateChildren()
    {
        for(int i = 0; i < 5; i++)
        {
            Particle* p = new Particle("./models/lavaBombModel.tri", direction, 2.0f, 1.0f);
            children.push_back(p);
        }
    }

    void Push(const Cartesian3& pushAmount)
    {
        Cartesian3 acceleration = pushAmount / mass;
        velocity = velocity + acceleration;
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
        // Update the children 
        for(int i = 0; i < children.size(); i++)
        {
            // Calculate the position of the child particle relative to the main particle
            Cartesian3 childPosition = position - direction * (i + 1) * 3.0f; // 10.0f is the distance between particles
            // Update the position and velocity of the child particle

            // Add a swirl to the trajectory of the child particles
            //float angle = dt * 2.0f * M_PI; // Rotate by one full circle (2π) every second
            
            auto angularSpeed = 1.0f / (i + 1);

            angle += angularSpeed * dt;

            float r = 10.f * (i + 1); // Radius of the swirl
            childPosition.x += r * cos(angle);
            childPosition.z += r * sin(angle);

            children[i]->position = childPosition;
            children[i]->velocity = velocity;
        }
        
        modelMatrix = view * columnMajorMatrix::Translate(position) * world * columnMajorMatrix::Scale(Cartesian3(radius, radius, radius));
    }

    float angle = 0.0f;
    HomogeneousFaceSurface lavaBombModel;
    HomogeneousFaceSurface Sphere;
    columnMajorMatrix modelMatrix;
    std::vector<Particle*> children;
};