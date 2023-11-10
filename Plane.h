#include <iostream>
#include "Particle.h"

// Define enum class so per plane object we can define it's role and
// adjust behaviour accordingly  
enum class PlaneRole
{
    AI,
    Controller
};
// Plane class defined can be used to create plane flying AI or player controlled plane
// Since we have a simple game and we know the exact objects the game will have we can use one class for both player 
// and in-game plane AI. Since the player is the plane, seperate classes will share the same attributes
// unnecessarily expanding the code base. Defining the class in it's current format allows us to expand
// the game in the future, to let the player control any plane in the scene
class Plane
{
public:
    // Take in the path to object, starting position, the size of the sphere around the object to detect collision and the role of the object
    Plane(const char *fileName, const Cartesian3& startPosition, float collisionRadius, bool clockwise, const PlaneRole& role);
    // Collision check functions to check if the plane collides with objects in the scene
    bool isCollidingWithAnotherPlane(const Plane& other);
    bool isCollidingWithParticle(const Particle& particle);
    bool isCollidingWithFloor(float height);

    // Update the movement of the plane each frame
    void Update(float dt, const columnMajorMatrix& worldMatrix, const columnMajorMatrix& viewMatrix);

    // Controls for the movement of the plane
    void Forward();
    void Back();
    void Right();
    void Left();
    void PitchUp();
    void PitchDown();
    void RollRight();
    void RollLeft();
    // Adjust scale of the plane object
    void SetScale(float s);
    // Increase and decrease speed for the plane movement
    void IncreaseSpeed();
    void DecreaseSpeed();
    // Set colour of the plane obejct
    void SetColor(float r, float g, float b, float a);
    
    // Getter for position, direction and up 
    Cartesian3 GetPostion() const { return m_position; }
    Cartesian3 GetDirection() const { return m_direction; }
    Cartesian3 GetUp() const { return m_up; }

    // Getters for yaw, pitch and roll to get these values
    float GetYaw() const { return m_yaw; }
    float GetPitch() const { return m_pitch; }
    float GetRoll() const { return m_roll; }

    // Get the current colour of the object
    const float* GetColor() { return planeColour; }
    // Get the value of the sphere radius around the plane used for collision
    float GetCollisionSphereRadius() const { return m_collisionSphereRadius; }

    // Keep these private for cleaner code when using these objects in SceneModel
    HomogeneousFaceSurface planeModel;
    columnMajorMatrix modelMatrix; // This is public since .Render method does not take a const reference, using getter would result in messier code

private:
    PlaneRole m_planeRole; // store the role of the plane, behaviour is different depending on what the plane should be having like
    Cartesian3 m_position; 
    Cartesian3 m_previousPosition;
    Cartesian3 m_forward;
    Cartesian3 m_direction;
    Cartesian3 m_up;

    // Properties for AI flying plane's in the world
    float m_scale = 500.0f; // 73
    float m_flightPathRadius = 3000.0f;
    float m_angle = 0.0f;
    float m_speed = 900.0f;
    bool m_clockWise;

    // Controllable player movement 
    float m_movementSpeed = 0.0f;
    float m_turnSpeed = 100.0f; // set turn speed quite high to allow for easy turning 
    float m_pitch = 0.0f;
    float m_yaw = 0.0f;
    float m_roll = 0.0f;

    float m_collisionSphereRadius = 86.0f; // default collision sphere radius
    float planeColour[4] = {0.5, 0.3, 0.0, 1.0};
    float deltaTime;
};