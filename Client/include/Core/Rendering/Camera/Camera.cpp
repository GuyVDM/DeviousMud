#include "precomp.h"
#include "Camera.h"


Camera::Camera(Utilities::ivec3 _pos, uint32_t _rd) : pos(_pos), render_distance(_rd)
{
}

Camera::Camera() : pos(Utilities::ivec3(0)), render_distance(0)
{
}

Camera::Camera(uint32_t _rd) : pos(Utilities::ivec3(0)), render_distance(_rd)
{
}

void Camera::set_position(const Utilities::ivec3& _pos)
{
    pos = _pos;
}

void Camera::set_render_distance(const uint32_t& _rd)
{
    render_distance = _rd;
}

const Utilities::ivec3& Camera::get_position() const
{
    return pos;
}

const uint32_t& Camera::get_render_distance() const
{
    return render_distance;
}


