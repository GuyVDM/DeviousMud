#pragma once
#include <vector>

class Animation2D
{
public:
	const std::vector<int>& get_keyframes() const;

	const int32_t get_last_frame_index() const;

public:
	explicit Animation2D(int32_t _startSpriteIndex, int32_t _endSpriteIndex);
	virtual ~Animation2D() = default;
	
private:
	std::vector<int> keyframes;
};