#include "precomp.h"

#include "Camera/Camera.h"

void Camera::Reset() 
{
	Position = Utilities::ivec2(0);
	Zoom = 1;
}

Camera::Camera() 
{
	Position = Utilities::ivec2(0);
}