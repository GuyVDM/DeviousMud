#pragma once
#include "Core/Core.hpp"

#include "Core/Renderer/SubSprite/SubSprite.hpp"

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
	Graphics::SpriteType SpriteType = Graphics::SpriteType::NONE;
	Utilities::ivec2     Position   = {1, 1};
	Utilities::ivec2     Size       = {1, 1};
	Color                Color      = {255,255,255,255};
	U8                   Frame      = 0;
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
	U32          Rows;
	U32          Columns;

	Sprite() noexcept : Texture(nullptr), Surface(nullptr), FrameCount(0), Rows(0), Columns(0) {};
	Sprite(SDL_Texture* _texture, SDL_Surface* _surface, const U8& _frameCount, const U32& _rows, const U32& _columns) noexcept
		: Texture(_texture), Surface(_surface), FrameCount(_frameCount), Rows(_rows), Columns(_columns) {};
};

struct SpriteFrameArgs 
{
	Utilities::ivec2 UV;
	Utilities::ivec2 IndividualSpriteSize;
	U32              FrameCount;
};

enum class e_TextAlignment 
{
	RIGHT = 0x00,
	MIDDLE,
	LEFT
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
	/// <summary>
	/// Transforms 2D screen coordinates into 2D worldspace coordinates.
	/// </summary>
	/// <param name="_screenCoords"></param>
	/// <returns></returns>
	static const Utilities::ivec2 ScreenToWorld(const Utilities::ivec2& _screenCoords);

public:
	/// <summary>
	/// Try find and return the sprite of a specified type.
	/// </summary>
	/// <param name="_type"></param>
	/// <returns></returns>
	Optional<Sprite> GetSprite(const Graphics::SpriteType& _type);

	/// <summary>
	/// Returns info about the sprite, such as how many rows and columns the sprite has.
	/// </summary>
	/// <param name="_subsprite"></param>
	/// <returns></returns>
	Optional<SpriteFrameArgs> GetSpriteFrameInfo(const SubSprite& _subsprite);

	/// <summary>
	/// Draws a filled rectangle in gridspace.
	/// </summary>
	/// <param name="_rect"></param>
	/// <param name="_col"></param>
	/// <param name="_zOrder"></param>
	void DrawRect(const SDL_Rect& _rect, const Color& _col, const U8& _zOrder = 0);

	/// <summary>
	/// Draws a rectangular outline in gridspace.
	/// </summary>
	/// <param name="_rect"></param>
	/// <param name="_col"></param>
	/// <param name="_outlWidth"></param>
	/// <param name="_zOrder"></param>
	void DrawRectOutline(const SDL_Rect& _rect, const Color& _col = {255,255,255,255}, const I32& _outlWidth = 1, const U8& _zOrder = 0);

	/// <summary>
	/// Refreshes & starts a new frame.
	/// </summary>
	void StartFrame();

	/// <summary>
	/// Plot visuals to the screen, be sure StartFrame() has been called first.
	/// </summary>
	/// <param name="_query"></param>
	/// <param name="_zOrder"></param>
	void Render(const RenderQuery& _query, const U8& _zOrder = 0);

	/// <summary>
	/// Render to screen.
	/// </summary>
	void EndFrame();

	/// <summary>
	/// Renders text to the screen.
	/// </summary>
	/// <param name="_args"></param>
	void RenderText(const TextArgs& _args, const float& _scale = 1.0f, const e_TextAlignment& _alignment = e_TextAlignment::RIGHT);

	/// <summary>
	/// Returns the SDL_Getwindow instance.
	/// </summary>
	/// <returns></returns>
	SDL_Window* GetWindow();

	/// <summary>
	/// Returns the SDL_Renderer instance.
	/// </summary>
	/// <returns></returns>
	SDL_Renderer* GetRenderer();

	/// <summary>
	/// Returns true or false based on if the rect is visible on the screen.
	/// </summary>
	/// <param name="_rect"></param>
	/// <returns></returns>
	const bool IsVisible(const SDL_Rect& _rect) const;

public:
	Renderer(const Utilities::ivec2& _windowSize);
	virtual ~Renderer();

private:
	void CreateSDLWindow(const I32& _width, const I32& _height);

	void LoadSprites(const Graphics::SpriteArgs& _args);

	void DrawGrid();

	void CreateRectTexture();

private:
	SDL_Window* m_Window;

	SDL_Renderer* m_Renderer;

	Ref<FontLoader> m_FontLoader;

	std::map<U8, std::vector<RenderQueryInstance>> m_RenderQuery;

	std::map<Graphics::SpriteType, Sprite> m_Sprites;

	SDL_Texture* m_RectTexture;
};