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
struct SDL_Window;
struct SDL_Rect;
struct Camera;

class FontLoader;

struct Color  
{
	U8 R, G, B, A;
};

enum e_TextureFlags : U8
{
	TEXTURE_NONE = 0x00,
	TEXTURE_DESTROY_AFTER_USE = 0x01
};

struct RenderQuery 
{
	Graphics::SpriteType Type;
	Utilities::ivec2     Position = {1, 1};
	Utilities::ivec2     Size     = {1, 1};
	Color                Color    = {255,255,255,255};
	U8                   Frame    = 0;
};

struct RenderQueryInstance : public RenderQuery
{
	SDL_Texture*   Texture;
	e_TextureFlags Flags;
};

struct Sprite
{
	SDL_Texture* Texture;
	SDL_Surface* Surface;
	U32          FrameCount;

	Sprite() : Texture(nullptr), Surface(nullptr), FrameCount(0) {};
	Sprite(SDL_Texture* _texture, SDL_Surface* _surface, const U8& _frameCount) : Texture(_texture), Surface(_surface), FrameCount(_frameCount) {};
};

struct TextArgs 
{
	Utilities::ivec2 Position;
	std::string      Text;
	U32              TextSize;
	Color            Color;
	U8               ZOrder;
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

	void RenderText(const TextArgs& _args);

	SDL_Window* GetWindow();

	SDL_Renderer* GetRenderer();

public:
	Renderer(const Utilities::ivec2& _windowSize);
	virtual ~Renderer();

private:
	void CreateSDLWindow(const I32& _width, const I32& _height);

	void LoadSprites(const Graphics::SpriteArgs& _args);

	void DrawGrid();

	void CreateRectTexture();

	const bool IsVisible(const SDL_Rect& _rect) const;

private:
	SDL_Window* m_Window;

	SDL_Renderer* m_Renderer;

	Ref<FontLoader> m_FontLoader;

	std::map<U8, std::vector<RenderQueryInstance>> m_RenderQuery;

	std::map<Graphics::SpriteType, Sprite> m_Sprites;

	SDL_Texture* m_RectTexture;
};