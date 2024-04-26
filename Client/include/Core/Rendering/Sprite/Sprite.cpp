#include "precomp.h"

#include "Core/Rendering/Sprite/Sprite.h"

using namespace DM::Utils;

const Graphics::SpriteType Graphics::_Sprite::get_sprite_type() const
{
	return m_eSpriteType;
}

const uint32_t Graphics::_Sprite::get_framecount() const
{
	return m_frameCount;
}

const UUID Graphics::_Sprite::get_uuid() const
{
	return m_uuid;
}

const float& Graphics::_Sprite::get_width() const
{
	return m_dimension.x;
}

const float& Graphics::_Sprite::get_height() const
{
	return m_dimension.y;
}

const Utilities::vec2& Graphics::_Sprite::get_dimensions() const
{
	return m_dimension;
}

Graphics::_Sprite::_Sprite() : 
	m_eSpriteType(SpriteType::NONE), m_frameCount(-1), color({255, 255, 255, 255}), m_uuid(UUID::generate()), m_dimension(0)
{
}

Graphics::_Sprite::~_Sprite()
{
	on_destroyed.invoke(m_uuid);
}

Graphics::_Sprite::_Sprite(const _Sprite& _other) :
	m_eSpriteType(_other.m_eSpriteType), m_frameCount(_other.m_frameCount), color(_other.color),
	m_uuid(UUID::generate()), m_dimension(_other.m_dimension)
{
}
