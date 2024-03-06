#include "precomp.h"
#include "Camera.h"


Camera::Camera(Utilities::vec2 _pos, uint32_t _rd) : pos(_pos), render_distance(_rd)
{
}

Camera::Camera() : pos(Utilities::vec2(0)), render_distance(0)
{
}

Camera::Camera(uint32_t _rd) : pos(Utilities::vec2(0)), render_distance(_rd)
{
}

void Camera::set_position(const Utilities::vec2& _pos)
{
    pos = _pos;
}

void Camera::set_render_distance(const uint32_t& _rd)
{
    render_distance = _rd;
}

const Utilities::vec2& Camera::get_position() const
{
    return pos;
}

const uint32_t& Camera::get_render_distance() const
{
    return render_distance;
}


