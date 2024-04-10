#include "precomp.h"

#include "Core/UI/UIComponent/TextComponent/TextComponent.h"

#include "Core/Rendering/Renderer.h"

#include "Core/Global/C_Globals.h"

using namespace Graphics;

std::shared_ptr<TextComponent> TextComponent::create_text(std::string _contents, Utilities::vec2 _pos, const TextArgs _args)
{
	std::shared_ptr<Graphics::Renderer> renderer = g_globals.renderer.lock();

	//Create font path
	std::string fontPath = renderer->assetsPath;
	{
		fontPath.append("/fonts/");
		fontPath.append(Fonts::FontMap().at(_args.font));
	}

	DEVIOUS_LOG(fontPath.c_str());

	//Load font
	TTF_Font* font = TTF_OpenFont(fontPath.c_str(), _args.size);
	DEVIOUS_ASSERT(font != NULL);

	//Create surface from font
	SDL_Surface* textSurface = TTF_RenderText_Solid(font, _contents.c_str(), _args.color);
	DEVIOUS_ASSERT(textSurface != NULL);

	std::shared_ptr<TextComponent> textComponent = std::make_shared<TextComponent>
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

	return textComponent;
}

TextComponent::~TextComponent()
{
	SDL_DestroyTexture(textTexture);
}

void TextComponent::render(std::shared_ptr<Graphics::Renderer> _renderer)
{
	UIComponent::render(_renderer);

	// Render the text.
	{
		const Utilities::ivec2 position = Utilities::to_ivec2(get_position());
		const Utilities::ivec2 size = Utilities::to_ivec2(get_size());
		const SDL_Rect renderQuad = { position.x, position.y, size.x, size.y };
		SDL_RenderCopy(_renderer->renderer, textTexture, NULL, &renderQuad);
	}
}
