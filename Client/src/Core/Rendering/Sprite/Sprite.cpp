#include "precomp.h"

#include "Core/Rendering/Sprite/Sprite.h"

using namespace DM::Utils;

const Graphics::SpriteType Graphics::_Sprite::get_sprite_type() const
{
	return m_eSpriteType;
}

const uint8_t Graphics::_Sprite::get_framecount() const
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

const uint32_t Graphics::_Sprite::get_rows() const
{
	return m_rows;
}

const uint32_t Graphics::_Sprite::get_columns() const
{
	return m_columns;
}

Graphics::_Sprite::_Sprite() : 
	m_eSpriteType(SpriteType::NONE), m_frameCount(-1), color({255, 255, 255, 255}), 
	m_uuid(UUID::generate()), m_dimension(0), m_rows(0), m_columns(0)
{
}

Graphics::_Sprite::_Sprite(const _Sprite& _other) :
	m_eSpriteType(_other.m_eSpriteType), m_frameCount(_other.m_frameCount), color(_other.color),
	m_uuid(UUID::generate()), m_dimension(_other.m_dimension), m_rows(_other.m_rows), m_columns(_other.m_columns)
{
}

Graphics::_Sprite::~_Sprite()
{
	on_destroyed.invoke(m_uuid);
}
