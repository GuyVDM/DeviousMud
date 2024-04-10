#include "precomp.h"

#include "Core/Rendering/Sprite/Sprite.h"

using namespace DM;
using namespace Utils;

const Graphics::SpriteType Graphics::_Sprite::get_sprite_type() const
{
	return spriteType;
}

const uint32_t Graphics::_Sprite::get_framecount() const
{
	return frameCount;
}

const DM::Utils::UUID Graphics::_Sprite::get_uuid() const
{
	return uuid;
}

const float& Graphics::_Sprite::get_width() const
{
	return dimension.x;
}

const float& Graphics::_Sprite::get_height() const
{
	return dimension.y;
}

const Utilities::vec2& Graphics::_Sprite::get_dimensions() const
{
	return dimension;
}

Graphics::_Sprite::_Sprite() : 
	spriteType(SpriteType::NONE), frameCount(-1), color({255, 255, 255, 255}), uuid(UUID::generate()), dimension(0)
{
}

Graphics::_Sprite::~_Sprite()
{
	on_destroyed.invoke(uuid);
}

Graphics::_Sprite::_Sprite(const _Sprite& _other) :
	spriteType(_other.spriteType), frameCount(_other.frameCount), color(_other.color), 
	uuid(UUID::generate()), dimension(_other.dimension)
{
}
