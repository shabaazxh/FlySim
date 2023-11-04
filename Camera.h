#include <iostream>
#include "Matrix4.h"
#include "Cartesian3.h"

class Camera
{
public:
    Camera(const Cartesian3& positon, const Cartesian3& direction) : m_position(positon), m_direction(direction),
    m_yaw(-90.0f), m_pitch(0.0f), m_dirChangeAmount(6.0f), m_movementSpeed(500.0f) {}

    Cartesian3 GetPosition() const { return m_position; }
    Cartesian3 GetDirection() const { return m_direction; }

    void Update()
    {
        if(m_pitch >= 89.0f) m_pitch = 89.0f;
        if(m_pitch <= -89.0f) m_pitch = -89.0f;

        Cartesian3 direction;
        direction.x = std::cos(DEG2RAD(m_yaw)) * std::cos(DEG2RAD(m_pitch));
        direction.y = std::sin(DEG2RAD(m_pitch));
        direction.z = std::sin(DEG2RAD(m_yaw)) * std::cos(DEG2RAD(m_pitch));
        m_direction = direction.unit();

        auto right = Cartesian3(0.0f, 1.0f, 0.0f).cross(m_direction);
        right = right.unit();

        m_up = m_direction.cross(right);
        m_up = m_up.unit();

        std::cout << "position: " << m_position << std::endl;

        FlyCamera();
    }

    void FlyCamera()
    {
        m_viewMatrix = columnMajorMatrix::constructView(m_position, (m_position + m_direction), m_up);
    }

    // Controls
    void Forward()
    {
        m_position = m_position + m_movementSpeed * m_direction;
    }

    void Back()
    {
        m_position = m_position - m_movementSpeed * m_direction;
    }

    void Right()
    {
        m_yaw += m_dirChangeAmount;
    }

    void Left()
    {
        m_yaw -= m_dirChangeAmount;
    }

    void PitchUp()
    {
        m_pitch += m_dirChangeAmount;
    }

    void PitchDown()
    {
        m_pitch -= m_dirChangeAmount;
    }

    float GetPitch() const { return m_pitch; }
    columnMajorMatrix GetViewMatrix() { return m_viewMatrix; }
private:
    Cartesian3 m_position;
    Cartesian3 m_direction;
    Cartesian3 m_up;
    float m_pitch;
    float m_yaw;
    columnMajorMatrix m_viewMatrix;
    float m_movementSpeed;
    float m_dirChangeAmount;
};