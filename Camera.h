#include <iostream>
#include "Matrix4.h"
#include "Cartesian3.h"

// Define enum class to determine how the camera should behave depending on it's type
enum class CameraMode
{
    Pilot,
    Follow
};

// Camera class to create a camera in the world to look from 
class Camera
{
public:
    // Camera constructor takes in position, direction and the type of camera it is. Sets all member variables appropriately 
    Camera(const Cartesian3& positon, const Cartesian3& direction, const CameraMode& mode) : m_position(positon), m_direction(direction),
    m_cameraMode(mode), m_up(Cartesian3(0, 1, 0)), m_yaw(-90.0f), m_pitch(0.0f), m_roll(0.0f), m_dirChangeAmount(6.0f), m_movementSpeed(500.0f),
    isThirdPersonEnabled(false) {}

    void Update();

    Cartesian3 GetPosition() const { return m_position; }
    Cartesian3 GetDirection() const { return m_direction; }
    Cartesian3 GetUp() const { return m_up; }
    float GetPitch() const { return m_pitch; }
    float GetYaw() const { return m_yaw; }
    columnMajorMatrix GetViewMatrix() { return m_viewMatrix; }
    CameraMode GetCameraMode() const { return m_cameraMode; }
    bool isThirdPerson() const { return isThirdPersonEnabled; }

    void EnableThirdPerson();
    void SetDirection(const Cartesian3& dir);
    void SetPosition(const Cartesian3& pos);
    void SetUp(const Cartesian3& up);
    void SetRotations(float yaw, float pitch, float roll);
    void SetCameraMode(const CameraMode& mode);

private:
    Cartesian3 m_position;
    Cartesian3 m_direction;
    Cartesian3 m_up;
    float m_pitch;
    float m_yaw;
    float m_roll;
    columnMajorMatrix m_viewMatrix;
    float m_movementSpeed;
    float m_dirChangeAmount;
    CameraMode m_cameraMode;
    bool isThirdPersonEnabled;
};