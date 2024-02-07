#include "precomp.h"
#include "Sprite.h"

const uint64_t Graphics::_Sprite::get_handle() const
{
	return handle;
}

const uint64_t Graphics::_Sprite::get_framecount() const
{
	return framecount;
}

Graphics::_Sprite::_Sprite() : 
	handle(-1), framecount(-1), color({})
{
}

Graphics::_Sprite::_Sprite(const uint64_t _handle, const uint32_t _framecount, const SDL_Color& _color) :
	handle(_handle), framecount(_framecount), color(_color) 
{

}

Graphics::_Sprite::_Sprite(const _Sprite& _other) :
	handle(_other.handle), framecount(_other.framecount), color(_other.color)
{
}
