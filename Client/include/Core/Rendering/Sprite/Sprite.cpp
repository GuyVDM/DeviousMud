#include "precomp.h"

#include "Core/Rendering/Sprite/Sprite.h"

using namespace DEVIOUSMUD;
using namespace RANDOM;

const Graphics::SpriteType Graphics::_Sprite::get_sprite_type() const
{
	return spriteType;
}

const uint32_t Graphics::_Sprite::get_framecount() const
{
	return frameCount;
}

const DEVIOUSMUD::RANDOM::UUID Graphics::_Sprite::get_uuid() const
{
	return uuid;
}

const float& Graphics::_Sprite::get_width() const
{
	return size.x;
}

const float& Graphics::_Sprite::get_height() const
{
	return size.y;
}

const Utilities::vec2& Graphics::_Sprite::get_dimensions() const
{
	return size;
}

Graphics::_Sprite::_Sprite() : 
	spriteType(SpriteType::NONE), frameCount(-1), color({255, 255, 255, 255}), uuid(UUID::generate()), size(0)
{
}

Graphics::_Sprite::_Sprite(const _Sprite& _other) :
	spriteType(_other.spriteType), frameCount(_other.frameCount), color(_other.color), 
	uuid(UUID::generate()), size(_other.size)
{
}
