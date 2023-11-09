#include <iostream>
#include <cstdlib>
#include <ctime>
#include "Matrix4.h"
class Particle
{
public:
    Particle(const char *fileName, const Cartesian3& direction, float speed, float r)
    {
        lavaBombModel.ReadFileTriangleSoup(fileName);
        Sphere.ReadFileTriangleSoup(fileName);

        this->direction = direction;
        position = Cartesian3(-38500.0f, 1000.0f, -4000); // default position
        velocity.x = direction.x;
        velocity.y = direction.y;
        velocity.z = direction.z;
        life = 10000.0f; // life of the particle
        radius = r;
        gravity = -19.81f;
        mass = 1.0f;
        shouldRender = true;
        std::srand(std::time(nullptr));
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

    // Create child objects of main particle to create smoke effect
    void CreateChildren()
    {
        for(int i = 0; i < 5; i++)
        {
            Particle* p = new Particle("./models/lavaBombModel.tri", direction, 2.0f, 1.0f);
            children.push_back(p);
        }
    }

    // Push a particle in the air. Used for collision between particles
    void Push(const Cartesian3& pushAmount)
    {
        Cartesian3 acceleration = pushAmount / mass;
        velocity = velocity + acceleration;
    }

    // Check if the current particle is colliding with another using sphere collision 
    bool isColliding(const Particle& other) const
    {
        float d = (position - other.GetPosition()).length();
        return d < (collisionSphereRadius + other.GetCollisionSphereRadius());
    }
    
    // Check if the particle is colliding with the floor
    bool isCollidingWithFloor(float height)
    {   
        return position.y - radius <= height;
    }

    // Update particle data each frame to ensure the particle physics + movement are correct 
    void update(float dt, const columnMajorMatrix& worldMatrix, const columnMajorMatrix& viewMatrix)
    {

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
            Cartesian3 childPosition = position - direction * (i + 1) * 1.0f; // 10.0f is the distance between particles
            // Update the position and velocity of the child particle

            // Add a swirl to the trajectory of the child particles
            //float angle = dt * 2.0f * M_PI; // Rotate by one full circle (2π) every second

            // random angle
            // Orient the child particle to align with the direction of the main particle 
            float angle = atan2(direction.z, direction.x) - M_PI / 2.0f;
            // introduce some randomness 
            float randomAngle = std::rand() % 360; // get a random angle in degrees
            float randomAngleRad = randomAngle * M_PI / 180.0f; // convert random angle to radians

            // add the randomness of the angle to give variation
            angle += (randomAngleRad * dt);

            float r = 500.0f * (i + 1); // define some radius to give a swirl
            //Apply it to the position to create a kind of smoke effect behind the main particle
            childPosition.x += r * cos(angle);
            childPosition.z += r * sin(angle);

            children[i]->position = childPosition;
            children[i]->velocity = velocity;
        }
        
        modelMatrix = viewMatrix * columnMajorMatrix::Translate(position) * worldMatrix * columnMajorMatrix::Scale(Cartesian3(radius, radius, radius));
    }

    Cartesian3 GetPosition() const { return position; }
    Cartesian3 GetDirection() const  { return direction; }
    float GetLife() const { return life; }
    void SetLife(float value) { life = value; }
    void ReduceLife(float amount) { life -= amount; }

    std::vector<Particle*> GetChildren() const { return children; }

    columnMajorMatrix GetModelMatrix() const { return modelMatrix; }
    void SetModelMatrix(columnMajorMatrix& mat) { modelMatrix = mat; }

    float GetCollisionSphereRadius() const { return collisionSphereRadius; }

    const GLfloat* GetColor() { return lavaBombColour; }
    const GLfloat* GetChildColor() { return childSmoke; }
    void SetColor(float r, float g, float b, float a)
    {
        lavaBombColour[0] = r;
        lavaBombColour[1] = g;
        lavaBombColour[2] = b;
        lavaBombColour[3] = a;
    }

    void SetScale(float scale)
    {
        radius = scale;
    }

    float GetScale() const { return radius; }
    bool GetShouldRender() const { return shouldRender; }
    void SetShouldRender(bool value) { shouldRender = value; }

    // Rendering models remain public 
    HomogeneousFaceSurface lavaBombModel;
    HomogeneousFaceSurface Sphere;
private:

    Cartesian3 position;
    Cartesian3 velocity;
    Cartesian3 direction;
    float life;
    float mass;
    //-9.81
    float gravity;
    float radius = 0.0f;
    float collisionSphereRadius = 86.0f;
    float angle = 0.0f;
    GLfloat lavaBombColour[4] = {0.5, 0.3, 0.0, 1.0};
    GLfloat childSmoke[4] = {1.0f, 1.0f, 1.0f, 1.0};

    columnMajorMatrix modelMatrix;
    std::vector<Particle*> children;
    bool shouldRender;
};