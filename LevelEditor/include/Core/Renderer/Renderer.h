#pragma once
#include "Core/Core.hpp"

#include "Shared/Utilities/vec2.hpp"

#include <unordered_map>
#include <vector>

namespace Graphics
{
	struct SpriteArgs;
	enum class SpriteType : uint16_t;
}

struct SDL_Texture;
struct SDL_Surface;
struct SDL_Renderer;
struct SDL_Rect;
struct Camera;

struct Color  
{
	U8 R, G, B, A;
};

struct RenderQuery 
{
	Graphics::SpriteType Type;
	Utilities::ivec2     Position = {1, 1};
	Utilities::ivec2     Size     = {1, 1};
	Color                Color    = {255,255,255,255};
	U8                   Frame    = 0;
};

enum e_TextureFlags : U8
{
	TEXTURE_NONE = 0x00,
	TEXTURE_DESTROY_AFTER_USE = 0x01
};

struct RenderQueryInstance : public RenderQuery
{
	SDL_Texture*   Texture;
	e_TextureFlags Flags;
	char DebugChar = ' ';
};

struct Sprite
{
	SDL_Texture* Texture;
	SDL_Surface* Surface;
	U8           FrameCount;

	Sprite() : Texture(nullptr), Surface(nullptr), FrameCount(0) {};
	Sprite(SDL_Texture* _texture, SDL_Surface* _surface, const U8& _frameCount) : Texture(_texture), Surface(_surface), FrameCount(_frameCount) {};
};

class Renderer 
{
public:
	static const Utilities::ivec2 ScreenToWorld(const Utilities::ivec2& _screenCoords);

public:
	const Optional<Sprite> GetSprite(const Graphics::SpriteType& _type);

	void DrawRect(const SDL_Rect& _rect, const Color& _col, const U8& _zOrder = 0);

	void DrawRectOutline(const SDL_Rect& _rect, const Color& _col = {255,255,255,255}, const I32& _outlWidth = 1, const U8& _zOrder = 0);

	void StartFrame();

	void Render(const RenderQuery& _query, const U8& _zOrder = 0);

	void EndFrame();

public:
	Renderer(SDL_Renderer* _renderer);
	virtual ~Renderer();

private:
	void LoadSprites(const Graphics::SpriteArgs& _args);

	void DrawGrid();

	void CreateRectTexture();

	const bool IsVisible(const SDL_Rect& _rect) const;

private:
	SDL_Renderer* m_Renderer;

	std::map<U8, std::vector<RenderQueryInstance>> m_RenderQuery;

	std::map<Graphics::SpriteType, Sprite> m_Sprites;

	SDL_Texture* m_RectTexture;
};