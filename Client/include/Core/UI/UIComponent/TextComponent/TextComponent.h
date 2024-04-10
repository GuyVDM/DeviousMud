#pragma once
#include "Core/UI/UIComponent/UIComponent.h"

#include "Core/Rendering/Fonts/FontsConfig.h"

//void Graphics::Renderer::render_text(std::string _contents, Utilities::vec2 _pos, TextArgs _args)
//{
//std::string fontPath = assetsPath;
//fontPath.append("/fonts/");
//fontPath.append(Fonts::FontMap().at(_args.font));
//
//TTF_Font* font = TTF_OpenFont(fontPath.c_str(), _args.size);
//if (font == NULL)
//{
//	DEVIOUS_ERR("Failed to load font! SDL_ttf Error: " << TTF_GetError());
//	return;
//}
//
//SDL_Surface* textSurface = TTF_RenderText_Solid(font, _contents.c_str(), _args.color);
//
//if (textSurface == NULL)
//{
//	DEVIOUS_ERR("Unable to render text surface! SDL_ttf Error: " << TTF_GetError());
//	TTF_CloseFont(font);
//	return;
//}
//
//SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, textSurface);
//if (texture == NULL)
//{
//	DEVIOUS_ERR("Unable to create texture from rendered text! SDL Error: " << SDL_GetError());
//	SDL_FreeSurface(textSurface);
//	TTF_CloseFont(font);
//	return;
//}
//
//SDL_FreeSurface(textSurface);
//TTF_CloseFont(font);
////}

struct SDL_Texture;

struct TextArgs
{
	SDL_Color  color;
	int        size;
	e_FontType font;

	const static TextArgs Default()
	{
		TextArgs args{};
		args.color = { 255, 255, 255, 255 };
		args.size = 10;
		args.font = e_FontType::RUNESCAPE_UF;
		return args;
	}
};

namespace Graphics
{
	class TextComponent final : public UIComponent
	{
	public:
		static std::shared_ptr<TextComponent> create_text(std::string _contents, Utilities::vec2 _pos, const TextArgs _args = TextArgs::Default());

		virtual ~TextComponent();

	private:
		using UIComponent::UIComponent;

		SDL_Texture* textTexture;

		virtual void render(std::shared_ptr<Graphics::Renderer> _renderer) override;

		friend class TextComponent;
		friend class std::shared_ptr<TextComponent>;
	};
}