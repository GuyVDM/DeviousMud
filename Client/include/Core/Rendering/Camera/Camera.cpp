#include "precomp.h"
#include "Camera.h"


Camera::Camera(Utilities::vec2 _pos, uint32_t _rd) : m_pos(_pos), m_renderDistance(_rd)
{
}

Camera::Camera() : m_pos(Utilities::vec2(0)), m_renderDistance(0)
{
}

Camera::Camera(uint32_t _rd) : m_pos(Utilities::vec2(0)), m_renderDistance(_rd)
{
}

void Camera::set_position(const Utilities::vec2& _pos)
{
    m_pos = _pos;
}

void Camera::set_render_distance(const uint32_t& _rd)
{
    m_renderDistance = _rd;
}

const Utilities::vec2& Camera::get_position() const
{
    return m_pos;
}

const uint32_t& Camera::get_render_distance() const
{
    return m_renderDistance;
}


