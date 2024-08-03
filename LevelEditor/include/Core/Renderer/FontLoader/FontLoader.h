#pragma once
#include "Core/Core.h"

typedef struct _TTF_Font TTF_Font;
struct SDL_Texture;
struct SDL_Surface;
struct SDL_Renderer;

class FontLoader
{
public:
	SDL_Texture* TextToTexture(SDL_Renderer* _renderer, const char* _contents, const U32& _textSize);

public:
	FontLoader() = default;
	FontLoader(const char* _filePath);
	~FontLoader();

private:
	void LoadFont();

private:
	bool         m_bLoadedFont = false;
	U32          m_FontSize = 10;
	TTF_Font*    m_FontPtr = nullptr;
	SDL_Texture* m_FontTexturePtr = nullptr;

	const char* m_FilePath = "";
};