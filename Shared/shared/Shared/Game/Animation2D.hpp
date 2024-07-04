#pragma once
#include <vector>

class Animation2D
{
public:
	inline const std::vector<int>& get_keyframes() const
	{
		return m_keyframes;
	};

	inline const int32_t get_last_frame_index() const
	{
		return static_cast<int32_t>(m_keyframes.size()) - 1;
	}

public:
	inline explicit Animation2D(int32_t _startSpriteIndex, int32_t _endSpriteIndex)
	{
		for (int i = _startSpriteIndex; i <= _endSpriteIndex; ++i)
		{
			m_keyframes.push_back(i);
		}
	};

	virtual ~Animation2D() = default;
	
private:
	std::vector<int> m_keyframes;
};