#include "precomp.h"
#include "Camera.h"


Camera::Camera(Utilities::vec2 _pos) : m_pos(_pos)
{
}

Camera::Camera() : m_pos(Utilities::vec2(0))
{
}

void Camera::set_position(const Utilities::vec2& _pos)
{
    m_pos = _pos;
}

const Utilities::vec2& Camera::get_position() const
{
    return m_pos;
}



