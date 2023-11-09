#include <iostream>
#include "Matrix4.h"
#include "Cartesian3.h"

enum class CameraMode
{
    Pilot,
    Follow
};

class Camera
{
public:
    Camera(const Cartesian3& positon, const Cartesian3& direction, const CameraMode& mode) : m_position(positon), m_direction(direction),
    m_cameraMode(mode), m_up(Cartesian3(0, 1, 0)), m_yaw(-90.0f), m_pitch(0.0f), m_roll(0.0f), m_dirChangeAmount(6.0f), m_movementSpeed(500.0f) {}

    Cartesian3 GetPosition() const { return m_position; }
    Cartesian3 GetDirection() const { return m_direction; }
    Cartesian3 GetUp() const { return m_up; }
    float GetPitch() const { return m_pitch; }
    float GetYaw() const { return m_yaw; }
    columnMajorMatrix GetViewMatrix() { return m_viewMatrix; }
    CameraMode GetCameraMode() const { return m_cameraMode; }

    void SetDirection(const Cartesian3& dir)
    {
        m_direction = dir;
    }

    void SetPosition(const Cartesian3& pos)
    {
        m_position = pos;
    }

    void SetUp(const Cartesian3& up)
    {
        m_up = up;
    }

    void SetRotations(float yaw, float pitch, float roll)
    {
        m_yaw = yaw;
        m_pitch = pitch;
        m_roll = roll;
    }

    void SetCameraMode(const CameraMode& mode)
    {
        m_cameraMode = mode;
    }

    void Update()
    {
        if(m_pitch >= 89.0f) m_pitch = 89.0f;
        if(m_pitch <= -89.0f) m_pitch = -89.0f;

        // auto up = Cartesian3(0, 1, 0);
        // auto forward = Cartesian3(1,0,0);
        // forward.Rotate(m_yaw, up);
        // forward = forward.unit();

        // Cartesian3 r = up;
        // r = r.cross(forward);
        // r = r.unit();
        // forward.Rotate(m_pitch, r);
        
        // Cartesian3 u = forward.cross(r);
        // u = u.unit();

        // m_direction = forward;
        // m_up = u;
        // columnMajorMatrix rot;
        // if(m_roll != 0)
        // {
        //     Quaternion roll(m_roll, forward);
        //     roll.Normalize();
        //     Quaternion result = roll * forward * roll.Conjugate();
        //     result.Normalize();
        //     rot = result.ToRotationMatrix();

        //     auto f = rot * Homogeneous4(forward.x, forward.y, forward.z, 1.0);
        //     auto upper = rot * Homogeneous4(up.x, up.y, up.z, 1.0);
        //     auto right = rot * Homogeneous4(r.x, r.y, r.z, 1.0);

        //     m_direction = Cartesian3(f.x, f.y, f.z);
        //     m_direction.unit();
        //     m_up = Cartesian3(upper.x, upper.y, upper.z);
        //     m_up.unit();
        //     r = Cartesian3(right.x, right.y, right.z);
        //     r.unit();
            
        //     // m_up = rot * up;
        //     // right = rot * right;
        // }

        // auto axis = Cartesian3(0.0f, 1.0f, 0.0f);
        // Quaternion rotation(180.0f, axis);
        // rotation.Normalize();
        // Quaternion result = rotation * offset * rotation.Conjugate();
        // result.Normalize();
        // columnMajorMatrix rot = result.ToRotationMatrix();

        if(m_cameraMode == CameraMode::Pilot)
        {
            // Cartesian3 direction;
            // direction.x = std::cos(DEG2RAD(m_yaw)) * std::cos(DEG2RAD(m_pitch));
            // direction.y = std::sin(DEG2RAD(m_pitch));
            // direction.z = std::sin(DEG2RAD(m_yaw)) * std::cos(DEG2RAD(m_pitch));
            // m_direction = direction.unit();


            // Camera was stretching with pitch but fixed it by recalculating
            // up vector which stopped the stretching
            auto right = Cartesian3(0.0f, 1.0f, 0.0f).cross(m_direction);
            right = right.unit();

            m_up = m_direction.cross(right);
            m_up = m_up.unit();

            m_up.Rotate(m_roll, m_direction);
            m_up = m_up.unit();
            
            // if(m_roll != 0)
            // {
            //     auto rollMatrix = columnMajorMatrix::Rotate(m_roll,
            //     m_direction);

            //     auto tempUp = rollMatrix * m_up;
            //     auto tempRight = rollMatrix * right;

            //     m_up = Cartesian3(tempUp.x, tempUp.y, tempUp.z);
            //     right = Cartesian3(tempRight.x, tempRight.y, tempRight.z);
            // }
            m_viewMatrix = columnMajorMatrix::constructView(m_position, (m_position + m_direction), m_up);
        } else 
        {
            m_viewMatrix = columnMajorMatrix::constructView(m_position, (m_position + m_direction), m_up);
        }

    }
    
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
};