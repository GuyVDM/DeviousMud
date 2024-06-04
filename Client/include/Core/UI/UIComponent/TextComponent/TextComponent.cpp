#include "precomp.h"

#include "Core/UI/UIComponent/TextComponent/TextComponent.h"

#include "Core/Rendering/Renderer.h"

#include "Core/Global/C_Globals.h"

using namespace Graphics;

std::shared_ptr<TextComponent> TextComponent::create_text(std::string _contents, Utilities::vec2 _pos, const TextArgs _args)
{
	std::shared_ptr<TextComponent> textComponent = UIComponent::create_component<TextComponent>
	(
			_pos,
			Utilities::vec2(1.0f),
			Graphics::SpriteType::NONE
	);

	textComponent->m_contents = _contents;
	textComponent->m_textArgs = _args;
	textComponent->rebuild_text();

	return textComponent;
}

TextComponent::~TextComponent()
{
	SDL_DestroyTexture(m_textTexture);
}

void Graphics::TextComponent::set_text(std::string _contents)
{
	m_contents = _contents;
	rebuild_text();
}

void Graphics::TextComponent::set_text_args(const TextArgs _args)
{
	m_textArgs = _args;
	rebuild_text();
}

void Graphics::TextComponent::init()
{
	on_render_call.add_listener
	(
		std::bind(&TextComponent::renderText, this, std::placeholders::_1)
	);
}

void Graphics::TextComponent::rebuild_text()
{
	if (m_textTexture)
	{
		SDL_DestroyTexture(m_textTexture);
	}

	if (!m_contents.empty())
	{
		std::shared_ptr<Graphics::Renderer> renderer = g_globals.renderer.lock();

		//Create font path
		std::string fontPath = renderer->m_assetsPath;
		{
			fontPath.append("/fonts/");
			fontPath.append(Fonts::FontMap().at(m_textArgs.font));
		}

		//Load font
		TTF_Font* font = TTF_OpenFont(fontPath.c_str(), m_textArgs.size);
		{
			DEVIOUS_ASSERT(font != NULL);

			//Create surface from font
			SDL_Surface* textSurface = TTF_RenderText_Solid(font, m_contents.c_str(), m_textArgs.color);
			DEVIOUS_ASSERT(textSurface != NULL);

			//Create texture from surface.
			SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer->m_renderer, textSurface);
			DEVIOUS_ASSERT(texture != NULL);
			SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

			Utilities::ivec2 textureDimensions{ 0 };
			SDL_QueryTexture(texture, NULL, NULL, &textureDimensions.x, &textureDimensions.y);

			//Set size equal to the texture.
			set_size(Utilities::to_vec2(textureDimensions));

			//Cleanup the font and surface.
			SDL_FreeSurface(textSurface);

			m_textTexture = texture;
		}
		TTF_CloseFont(font);
	}
}

void TextComponent::renderText(std::shared_ptr<Graphics::Renderer> _renderer)
{
	if (m_textTexture)
	{
		const Utilities::vec2 position = get_position();
		const Utilities::vec2 size     = get_size();

		float dropShadowOffsetPx = 1.0f;

		uint8_t flags = 0;
		if (m_eRenderMode == e_RenderMode::WORLDSPACE)
		{
			flags |= e_RenderMode::WORLDSPACE;
			dropShadowOffsetPx = 1.0f / (float)Renderer::GRID_CELL_PX_SIZE;
		}

		// Render the shadow text.
		if (m_textArgs.bDropShadow)
		{
			SpriteRenderData data{};
			data.color       = { 1, 1, 1, 255 };
			data.position    = position + Utilities::vec2(dropShadowOffsetPx);
			data.size        = size;
			data.texture     = m_textTexture;
			data.renderFlags = flags;

			_renderer->plot_texture(data, m_sprite.zRenderPriority);
		}

		// Render the main text.
		{
			SpriteRenderData data{};
			data.color       = m_textArgs.color;
			data.position    = position;
			data.size        = size;
			data.texture     = m_textTexture;
			data.renderFlags = flags;

			_renderer->plot_texture(data, m_sprite.zRenderPriority);
		}
	}
}
