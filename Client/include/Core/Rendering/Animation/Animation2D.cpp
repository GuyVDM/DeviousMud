#include "precomp.h"

#include "Core/Rendering/Animation/Animation2D.h"

Animation2D::Animation2D(int32_t _startSpriteIndex, int32_t _endSpriteIndex)
{
	for (int i = _startSpriteIndex; i <= _endSpriteIndex; ++i)
	{
		keyframes.push_back(i);
	}
}

const std::vector<int>& Animation2D::get_keyframes() const
{
	return keyframes;
}

const int32_t Animation2D::get_last_frame_index() const
{
	return keyframes.size() - 1;
}


