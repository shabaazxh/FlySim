#include <iostream>
#include <cstdlib>
#include <ctime>
#include "Matrix4.h"
#include "HomogeneousFaceSurface.h"

class Particle
{
public:
    Particle(const char *fileName, const Cartesian3& direction, float speed, float s);
    ~Particle();

    void Push(const Cartesian3& pushAmount);
    void CreateChildren();
    bool isColliding(const Particle& other);
    bool isCollidingWithFloor(float height);
    void Update(float dt, const columnMajorMatrix& worldMatrix, const columnMajorMatrix& viewMatrix);
    
    Cartesian3 GetPosition() const { return position; }
    Cartesian3 GetDirection() const  { return direction; }
    std::vector<Particle*> GetChildren() const { return children; }
    columnMajorMatrix GetModelMatrix() const { return modelMatrix; }
    float GetCollisionSphereRadius() const { return collisionSphereRadius; }
    const float* GetColor() { return lavaBombColour; }
    const float* GetChildColor() { return childSmoke; }

    void SetModelMatrix(columnMajorMatrix& mat) { modelMatrix = mat; }
    void SetColor(float r, float g, float b, float a);
    void SetScale(float scale);
    float GetScale() const { return m_scale; }
    bool GetShouldRender() const { return shouldRender; }
    void SetShouldRender(bool value) { shouldRender = value; }

    // Rendering models remain public 
    HomogeneousFaceSurface lavaBombModel;
private:

    Cartesian3 position;
    Cartesian3 velocity;
    Cartesian3 direction;
    float mass;
    //-9.81
    float gravity;
    float m_scale = 0.0f;
    float collisionSphereRadius = 86.0f;
    float angle = 0.0f;
    float lavaBombColour[4] = {0.5, 0.3, 0.0, 1.0};
    float childSmoke[4] = {1.0f, 1.0f, 1.0f, 1.0};

    columnMajorMatrix modelMatrix;
    std::vector<Particle*> children;
    bool shouldRender;
};