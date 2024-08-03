#pragma once
#include "Core/Core.h"

#include "Shared/Game/SpriteTypes.hpp"


/// <summary>
/// Container in order to specify what part of the spritesheet you want to target.
/// </summary>
struct SubSprite
{
	Graphics::SpriteType SpriteType;
	U32                  Frame;

	inline SubSprite() noexcept
		: SpriteType(Graphics::SpriteType::NONE), Frame(0)
	{
	};

	inline SubSprite(const Graphics::SpriteType& _type, const U32& _frame) noexcept
		: SpriteType(_type), Frame(_frame)
	{
	};

	inline SubSprite operator=(const SubSprite& _other)
	{
		if(this != &_other) 
		{
			SpriteType = _other.SpriteType;
			Frame      = _other.Frame;
		}

		return *this;
	}

	inline bool operator==(const SubSprite& _other) const
	{
		return (SpriteType == _other.SpriteType) && (Frame == _other.Frame);
	}

	inline bool operator!=(const SubSprite& _other) const
	{
		return !(*this == _other);
	}
};