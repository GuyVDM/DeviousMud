#include "precomp.h"

#include "Core/Renderer/FontLoader/FontLoader.h"

SDL_Texture* FontLoader::TextToTexture(SDL_Renderer* _renderer, const char* _contents, const U32& _textSize)
{
	if (_textSize != m_FontSize)
	{
		m_FontSize = _textSize; 
		LoadFont();
	}

	constexpr SDL_Color color = { 255, 255, 255, 255 };
	SDL_Surface* surface = TTF_RenderText_Solid(m_FontPtr, _contents, color);
	if (!surface)
	{
		DEVIOUS_ERR("Unable to create surface: " << TTF_GetError());
		return nullptr;
	}

	SDL_Texture* texture = SDL_CreateTextureFromSurface(_renderer, surface);
	if (!texture)
	{
		DEVIOUS_ERR("Unable to create texture: " << TTF_GetError());
		return nullptr;
	}

	SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
	SDL_FreeSurface(surface);

	return texture;
}

void FontLoader::LoadFont()
{
	//Destroy previous font if it exists.
	if (m_bLoadedFont)
	{
		TTF_CloseFont(m_FontPtr);
	}

	m_FontPtr = TTF_OpenFont(m_FilePath, m_FontSize);
	m_bLoadedFont = m_FontPtr != nullptr ? true : false;

	if (!m_bLoadedFont)
	{
		DEVIOUS_ERR(TTF_GetError());
		return;
	}
}

FontLoader::FontLoader(const char* _filePath)
{
	m_FilePath = _filePath;
	LoadFont();
}

FontLoader::~FontLoader()
{
	if (m_bLoadedFont)
	{
		TTF_CloseFont(m_FontPtr);
	}
}