#include <iostream>
#include "Particle.h"

// Define enum class so per plane object we can define it's role and
// adjust behaviour automtically 
enum class PlaneRole
{
    Particle,
    Controller
};
// Plane class defined can be used to create plane flying AI or player controlled plane
// Since we have a simple game and we know the exact objects the game will have we can use one class for both player 
// and in-game plane AI
class Plane
{
public:
    // Take in the path to object, starting position, the size of the sphere around the object to detect collision and the role of the object
    Plane(const char *fileName, const Cartesian3& startPosition, float collisionRadius, const PlaneRole& role);
    // Collision check functions to check if the plane collides with objects in the scene
    bool isColliding(const Plane& other);
    bool isCollidingWithParticle(const Particle& particle);
    bool isCollidingWithFloor(float height);

    // Update the movement of the plane each frame
    void Update(float dt, const columnMajorMatrix& worldMatrix, const columnMajorMatrix& viewMatrix);

    void SetScale(float s);
    // Increase and decrease speed for the plane movement
    void IncreaseSpeed();
    void DecreaseSpeed();

    // Controls for the movement of the plane
    void Forward();
    void Back();
    void Right();
    void Left();
    void PitchUp();
    void PitchDown();
    void RollRight();
    void RollLeft();

    HomogeneousFaceSurface planeModel;
    HomogeneousFaceSurface Sphere;
    columnMajorMatrix modelMatrix;
    columnMajorMatrix sphereMatrix;
    PlaneRole m_planeRole; // store the role of the plane, behaviour is different depending on what the plane should be having like
    Cartesian3 position; 
    Cartesian3 previousPosition;
    Cartesian3 forward;
    Cartesian3 direction;
    Cartesian3 up;

    float scale = 500.0f; // 73
    float flightPathRadius = 3000.0f;
    float angle = 0.0f;
    float speed = 900.0f;

    // Controllable player movement 
    float movementSpeed = 0.0f;
    float turnSpeed = 500.0f;
    float pitch = 0.0f;
    float yaw = 0.0f;
    float roll = 0.0f;

    float collisionSphereRadius = 86.0f; // default collision sphere radius
    float lavaBombColour[4] = {0.5, 0.3, 0.0, 1.0};
    bool shouldRender = true;
    float deltaTime;
};