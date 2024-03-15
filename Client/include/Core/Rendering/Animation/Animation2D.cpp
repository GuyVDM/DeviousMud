#include "precomp.h"

#include "Core/Rendering/Animation/Animation2D.h"

Animation2D::Animation2D(std::vector<int> _keyFrames) : keyframes(_keyFrames)
{
}

const std::vector<int>& Animation2D::get_keyframes() const
{
	return keyframes;
}

const int32_t Animation2D::get_last_frame_index() const
{
	return keyframes.size() - 1;
}


