#include "precomp.h"

#include "Core/UI/UIComponent/TextComponent/TextComponent.h"

#include "Core/Rendering/Renderer.h"

#include "Core/Global/C_Globals.h"

#include <sstream>

#include <iomanip>

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
	clean();
}

std::vector<Graphics::TextComponent::TextSegmentColor> Graphics::TextComponent::split_with_color_codes(const std::string& _contents)
{
	std::vector<TextSegmentColor> textSegments;
	std::istringstream sstream(_contents);

	char ch;
	std::string currentText;
	SDL_Color currentColor = m_textArgs.color;

	while (sstream >> std::noskipws >> ch)
	{
		currentText += ch;

		//If a color code is being typed out
		const size_t colCodeLoc = currentText.find("<col=#");
		if(colCodeLoc != std::string::npos) 
		{
			if(currentText.size() >= colCodeLoc + 12) 
			{
				if(currentText[colCodeLoc + 12] == '>') 
				{
					std::string colCode = currentText.substr(colCodeLoc + 6 , 6);
					
					SDL_Color tempColor;

					if(parse_hex_to_color(colCode, tempColor))
					{
						currentText.erase(colCodeLoc, 13);
						
						textSegments.push_back({ currentText, currentColor });

						currentText.clear();

						currentColor = tempColor;
					}
				}
			}
		}
	}

	textSegments.push_back({ currentText, currentColor });

	return textSegments;
}

void Graphics::TextComponent::clean()
{
	for (SDL_Texture* texture : m_textTextures)
	{
		SDL_DestroyTexture(texture);
	}

	m_textTextures.clear();
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
	if (m_textTextures.size() > 0)
	{
		clean();
	}

	generate_text_textures();
}

const void Graphics::TextComponent::generate_text_textures()
{
	std::vector<TextSegmentColor> segments = split_with_color_codes(m_contents);

	for (auto& segment : segments) 
	{
		create_texture(segment.text, segment.color);
	}
}

void Graphics::TextComponent::create_texture(const std::string& _contents, const SDL_Color& _col)
{
	if (!_contents.empty())
	{
		std::shared_ptr<Graphics::Renderer> renderer = g_globals.renderer.lock();

		//Create font path
		std::string fontPath = renderer->m_assetsPath;
		{
			fontPath.append("/fonts/");
			fontPath.append(Fonts::FontMap().at(m_textArgs.font));
		}

		//Load font into a texture & close it afterwards.
		TTF_Font* font = TTF_OpenFont(fontPath.c_str(), m_textArgs.size);
		{
			DEVIOUS_ASSERT(font != NULL);

			SDL_Surface* textSurface = TTF_RenderText_Solid(font, _contents.c_str(), _col);
			DEVIOUS_ASSERT(textSurface != NULL);

			SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer->m_renderer, textSurface);
			DEVIOUS_ASSERT(texture != NULL);

			SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
			SDL_FreeSurface(textSurface);

			m_textTextures.push_back(texture);

			TTF_CloseFont(font);
		}
		
		//*------------------------------------------------------
		// Resize the text element incorporating all the textures.
		//*
		{
			Utilities::vec2 newSize = { 0.0f };
			for (int32_t i = 0; i < m_textTextures.size(); i++)
			{
				Utilities::ivec2 textureDimensions{ 0 };
				SDL_QueryTexture(m_textTextures[i], NULL, NULL, &textureDimensions.x, &textureDimensions.y);
				newSize.x += static_cast<float>(textureDimensions.x);
				newSize.y = std::max(newSize.y, static_cast<float>(textureDimensions.y));
			}

			set_size(newSize);
		}
	}
}

bool Graphics::TextComponent::parse_hex_to_color(const std::string& _string, SDL_Color& _color) const
{	
	DEVIOUS_LOG(_string);

	for(char c : _string)
	{
		//*--------------------------------------
		// If it's a valid hexadecimal character.
		//*
		if(!std::isxdigit(c)) 
		{
			return false;
		}
	}
	
	//*----------------------
	// Convert hex to integer
	//*
	std::istringstream iss(_string);
	uint32_t rgb;
	iss >> std::hex >> rgb;

	if (iss.fail()) 
	{
		return false;
	}

	_color.r = (rgb >> 16) & 0xFF;
	_color.g = (rgb >> 8) & 0xFF;
	_color.b =  rgb & 0xFF;

	return true;
}

void TextComponent::renderText(std::shared_ptr<Graphics::Renderer> _renderer)
{
	if (m_textTextures.size() > 0)
	{
		const Utilities::vec2 position = get_position();
		const Utilities::vec2 size = get_size();

		float dropShadowOffsetPx = 1.0f;

		uint8_t flags = 0;
		if (m_eRenderMode == e_RenderMode::WORLDSPACE)
		{
			flags |= e_RenderMode::WORLDSPACE;
			dropShadowOffsetPx = 1.0f / (float)Renderer::GRID_CELL_PX_SIZE;
		}

		Utilities::vec2  elementPosition = get_position();
		Utilities::ivec2 textureDimensions{ 0 };

		for (int32_t i = 0; i < m_textTextures.size(); i++)
		{
			SDL_QueryTexture(m_textTextures[i], NULL, NULL, &textureDimensions.x, &textureDimensions.y);

			// Render the shadow text.
			if (m_textArgs.bDropShadow)
			{
				SpriteRenderData data{};
				data.color    = { 1, 1, 1, 255 };
				data.position = elementPosition + Utilities::vec2(dropShadowOffsetPx);
				data.size     = Utilities::to_vec2(textureDimensions);
				data.texture  = m_textTextures[i];
				data.renderFlags = flags;

				_renderer->plot_texture(data, m_sprite.zRenderPriority);
			}

			// Render the main text.
			{
				SpriteRenderData data{};
				data.color = { 255, 255, 255, 255 };
				data.position = elementPosition;
				data.size     = Utilities::to_vec2(textureDimensions);
				data.texture  = m_textTextures[i];
				data.renderFlags = flags;

				_renderer->plot_texture(data, m_sprite.zRenderPriority);
			}

			elementPosition = elementPosition + Utilities::vec2
			(
				static_cast<float>(textureDimensions.x),
				0.0f
			);
		}
	}
}
