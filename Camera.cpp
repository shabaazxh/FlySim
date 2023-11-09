#include "Camera.h"

void Camera::Update()
{
    // Update the cameras up correctly if it's in pilot mode otherwise leave the values as they are
    if(m_cameraMode == CameraMode::Pilot)
    {
        // Camera was stretching with pitch but fixed it by recalculating
        // up vector which stopped the stretching
        auto right = Cartesian3(0.0f, 1.0f, 0.0f).cross(m_direction);
        right = right.unit();

        m_up = m_direction.cross(right);
        m_up = m_up.unit();

        m_up.Rotate(m_roll, m_direction);
        m_up = m_up.unit();
        
        m_viewMatrix = columnMajorMatrix::constructView(m_position, (m_position + m_direction), m_up);
    } else 
    {
        m_viewMatrix = columnMajorMatrix::constructView(m_position, (m_position + m_direction), m_up);
    }

}

void Camera::EnableThirdPerson()
{
    isThirdPerson = isThirdPerson == true ? false : true;
}

void Camera::SetDirection(const Cartesian3& dir)
{
    m_direction = dir;
}

void Camera::SetPosition(const Cartesian3& pos)
{
    m_position = pos;
}

void Camera::SetUp(const Cartesian3& up)
{
    m_up = up;
}

void Camera::SetRotations(float yaw, float pitch, float roll)
{
    m_yaw = yaw;
    m_pitch = pitch;
    m_roll = roll;
}

void Camera::SetCameraMode(const CameraMode& mode)
{
    m_cameraMode = mode;
}