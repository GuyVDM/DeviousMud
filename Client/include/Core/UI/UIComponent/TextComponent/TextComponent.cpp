#include "precomp.h"

#include "Core/UI/UIComponent/TextComponent/TextComponent.h"

#include "Core/Rendering/Renderer.h"

#include "Core/Global/C_Globals.h"

using namespace Graphics;

std::shared_ptr<TextComponent> TextComponent::create_text(std::string _contents, Utilities::vec2 _pos, const TextArgs _args)
{
	std::shared_ptr<TextComponent> textComponent;

	if (!_contents.empty())
	{

		std::shared_ptr<Graphics::Renderer> renderer = g_globals.renderer.lock();

		//Create font path
		std::string fontPath = renderer->assetsPath;
		{
			fontPath.append("/fonts/");
			fontPath.append(Fonts::FontMap().at(_args.font));
		}

		//Load font
		TTF_Font* font = TTF_OpenFont(fontPath.c_str(), _args.size);
		DEVIOUS_ASSERT(font != NULL);

		//Create surface from font
		SDL_Surface* textSurface = TTF_RenderText_Solid(font, _contents.c_str(), _args.color);
		DEVIOUS_ASSERT(textSurface != NULL);

		textComponent = UIComponent::create_component<TextComponent>
			(
				_pos,
				Utilities::vec2((float)textSurface->w, (float)textSurface->h),
				Graphics::SpriteType::NONE
			);

		//Create texture from surface.
		SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer->renderer, textSurface);
		DEVIOUS_ASSERT(texture != NULL);

		//Cleanup the font and surface.
		SDL_FreeSurface(textSurface);
		TTF_CloseFont(font);

		//Set text texture.
		textComponent->textTexture = texture;
		textComponent->optionArgs = _args;
	}
	else 
	{
		textComponent = UIComponent::create_component<TextComponent>
		(
			_pos,
			Utilities::vec2(1.0f, 1.0f),
			Graphics::SpriteType::NONE
		);
	}

	return textComponent;
}

TextComponent::~TextComponent()
{
	SDL_DestroyTexture(textTexture);
}

void Graphics::TextComponent::init()
{
	on_render_call.add_listener
	(
		std::bind(&TextComponent::renderText, this, std::placeholders::_1)
	);
}

void TextComponent::renderText(std::shared_ptr<Graphics::Renderer> _renderer)
{
	if (textTexture)
	{
		const Utilities::ivec2 position = Utilities::to_ivec2(get_position());
		const Utilities::ivec2 size = Utilities::to_ivec2(get_size());

		if (optionArgs.bDropShadow)
		{
			const static int32_t dropShadowOffsetPx = 1;
			const SDL_Rect dsRenderQuad = { position.x + dropShadowOffsetPx, position.y + dropShadowOffsetPx, size.x, size.y };

			SDL_SetTextureColorMod(textTexture, 0, 0, 0);
			SDL_SetTextureAlphaMod(textTexture, optionArgs.color.a);

			SDL_RenderCopy(_renderer->renderer, textTexture, NULL, &dsRenderQuad);

			SDL_SetTextureColorMod(textTexture, optionArgs.color.r, optionArgs.color.g, optionArgs.color.b);
			SDL_SetTextureAlphaMod(textTexture, optionArgs.color.a);
		}

		// Render the text.
		{
			const SDL_Rect renderQuad = { position.x, position.y, size.x, size.y };
			SDL_RenderCopy(_renderer->renderer, textTexture, NULL, &renderQuad);
		}
	}
}
