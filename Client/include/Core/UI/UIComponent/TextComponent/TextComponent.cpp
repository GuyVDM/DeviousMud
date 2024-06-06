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

std::vector<Graphics::TextComponent::TextSegment> Graphics::TextComponent::split_text_to_segments(const std::string& _contents) const
{
	std::vector<TextSegment> textSegments;
	std::istringstream sstream(_contents);

	char ch;
	std::string currentText;
	SDL_Color currentColor = m_textArgs.color;

	while (sstream >> std::noskipws >> ch)
	{
		currentText += ch;

		//*----------------------------------
		// If a icon code is being typed out.
		//*
		const size_t iconStart = currentText.find("<icon=");
		size_t iconEnd = -1;

		if (iconStart != std::string::npos)
		{
			if (currentText.size() >= iconStart + 7)
			{
				iconEnd = currentText.find(">");

				if (iconEnd != std::string::npos)
				{
					//If the icon end character is after icon start
					//It msut be valid.
					if (iconEnd > iconStart)
					{
						std::string iconCode = currentText.substr(iconStart, iconEnd - iconStart + 1);

						//Segment the first part before the code first.
						if(iconStart > 0) 
						{
							textSegments.push_back({ currentText.substr(0, iconStart), currentColor });
						}

						textSegments.push_back({ iconCode, currentColor });
						currentText.clear();
						continue;
					}
				}
			}
		}

		//*---------------------------------
		//If a color code is being typed out.
		//
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

bool Graphics::TextComponent::parse_hex_to_color(const std::string& _string, SDL_Color& _color)
{
	for (char c : _string)
	{
		//*--------------------------------------
		// If it's a valid hexadecimal character.
		//*
		if (!std::isxdigit(c))
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
	_color.b = rgb & 0xFF;

	return true;
}

bool Graphics::TextComponent::is_icon(const std::string& _contents, int32_t& _iconId)
{
	const size_t iconStart = _contents.find("<icon=");
	size_t iconEnd = -1;

	if (iconStart != std::string::npos)
	{
		if (_contents.size() >= iconStart + 7)
		{
			iconEnd = _contents.find(">");

			if (iconEnd != std::string::npos)
			{
				//If the icon end character is after icon start
				//It must be valid.
				if (iconEnd > iconStart)
				{
					std::string iconCode = _contents.substr(iconStart + 6, (iconEnd - (iconStart + 6)));

					try
					{
						int32_t iconId = std::stoi(iconCode);

						if(iconId > -1 && iconId < ICON_COUNT)
						{
							_iconId = iconId;
							return true;
						}

					}
					catch (std::invalid_argument& e)
					{
						DEVIOUS_ERR("Error occured: " << e.what());
						return false;
					}
					catch (std::out_of_range& e)
					{
						DEVIOUS_ERR("Error occured: " << e.what());
						return false;
					}
				}
			}
		}
	}

	return false;
}

void Graphics::TextComponent::clean()
{
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

void Graphics::TextComponent::on_new_parent()
{
	rebuild_text();
}

void Graphics::TextComponent::rebuild_text()
{
	//*-------------------------------------------
	//Check first if there's any text to be built.
	//
	if (m_textArgs.size > 0 && !m_contents.empty())
	{
		if (m_textTextures.size() > 0)
		{
			clean();
		}

		std::shared_ptr<Graphics::Renderer> renderer = g_globals.renderer.lock();

		//Reconstruct fontpath.
		{
			m_fontPath = renderer->m_assetsPath;
			m_fontPath.append("/fonts/");
			m_fontPath.append(Fonts::FontMap().at(m_textArgs.font));
		}

		//*------------------------------------------------------
		// Load font, generate textures and close font afterwards
		//
		m_font = TTF_OpenFont(m_fontPath.c_str(), m_textArgs.size);

		if (m_font)
		{
			int32_t fontHeight = TTF_FontHeight(m_font);
			m_iconRenderSize = Utilities::vec2(static_cast<float>(fontHeight));

			DEVIOUS_ASSERT(m_font != NULL);
			generate_text_textures();

			TTF_CloseFont(m_font);
		}

		//*------------------------------------------------------
		// Resize the text element incorporating all the textures.
		// And determine position of each segment.
		//*
		{
			Utilities::vec2 newSize = { 0.0f };
			Utilities::ivec2 textureDimensions{ 0 };
			for (int32_t i = 0; i < m_textTextures.size(); i++)
			{
				auto& query = m_textTextures[i];

				if (!query->bIsIcon)
				{
					SDL_QueryTexture(query->tex, NULL, NULL, &textureDimensions.x, &textureDimensions.y);
					newSize.x += static_cast<float>(textureDimensions.x);
					newSize.y = std::max(newSize.y, static_cast<float>(textureDimensions.y));
				}
				else
				{
					newSize.x += m_iconRenderSize.x;
					newSize.y = std::max(newSize.y, m_iconRenderSize.y);
				}
			}

			set_size(newSize);
		}

		//*------------------------------------------------------
		// Calculate what the render positions should be for the textures.
		//*
		{
			Utilities::vec2 offset(0.0f);

			for (int32_t i = 0; i < m_textTextures.size(); i++)
			{
				auto& query = m_textTextures[i];
				query->renderPosOffset = offset;

				float textureOffsetX = 0.0f;

				if (!query->bIsIcon)
				{
					Utilities::ivec2 textureDimensions{ 0 };
					SDL_QueryTexture(query->tex, NULL, NULL, &textureDimensions.x, &textureDimensions.y);

					textureOffsetX = static_cast<float>(textureDimensions.x);
				}
				else
				{
					textureOffsetX += m_iconRenderSize.x;
				}

				if(m_eRenderMode == e_RenderMode::WORLDSPACE) 
				{
					textureOffsetX /= static_cast<float>(Graphics::Renderer::GRID_CELL_PX_SIZE);
				}

				offset.x += textureOffsetX;
			}
		}
	}
}

const void Graphics::TextComponent::generate_text_textures()
{
	std::vector<TextSegment> segments = split_text_to_segments(m_contents);

	for (auto& segment : segments) 
	{
		create_texture(segment.text, segment.color);
	}
}

void Graphics::TextComponent::create_texture(const std::string& _contents, const SDL_Color& _col)
{
	if (!_contents.empty() && m_textArgs.size > 0)
	{
		std::shared_ptr<Graphics::Renderer> renderer = g_globals.renderer.lock();

		std::shared_ptr<RenderQuery> queryPtr = std::make_shared<RenderQuery>();

		int32_t iconId = -1;
		if (is_icon(_contents, iconId))
		{
			Graphics::SpriteType iconType = static_cast<Graphics::SpriteType>(iconId);

			queryPtr->bIsIcon = true;
			queryPtr->icon = renderer->get_sprite(iconType);
			queryPtr->tex = nullptr;

			queryPtr->icon.zRenderPriority = m_sprite.zRenderPriority;

			m_textTextures.push_back(queryPtr);
		}
		else
		{
			SDL_Surface* textSurface = TTF_RenderText_Solid(m_font, _contents.c_str(), _col);
			DEVIOUS_ASSERT(textSurface != NULL);

			SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer->m_renderer, textSurface);

			Utilities::ivec2 textureDimensions{ 0 };
			SDL_QueryTexture(texture, NULL, NULL, &textureDimensions.x, &textureDimensions.y);

			DEVIOUS_ASSERT(texture != NULL);

			SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
			SDL_FreeSurface(textSurface);

			queryPtr->bIsIcon = false;
			queryPtr->tex     = texture;
			queryPtr->icon    = g_globals.renderer.lock()->get_sprite(Graphics::SpriteType::NONE);

			m_textTextures.push_back(std::shared_ptr<RenderQuery>(queryPtr));
		}
	}
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

		Utilities::ivec2 textureDimensions{ 0 };

		for (int32_t i = 0; i < m_textTextures.size(); i++)
		{
			auto& query = m_textTextures[i];

			Utilities::vec2  segmentPosition = position + query->renderPosOffset;

			if (!query->bIsIcon)
			{
				SDL_QueryTexture(query->tex, NULL, NULL, &textureDimensions.x, &textureDimensions.y);

				// Render the shadow text.
				if (m_textArgs.bDropShadow)
				{
					SpriteRenderData data{};
					data.color = { 1, 1, 1, 255 };
					data.position = segmentPosition + Utilities::vec2(dropShadowOffsetPx);
					data.size = Utilities::to_vec2(textureDimensions);
					data.texture = query->tex;
					data.renderFlags = flags;

					_renderer->plot_texture(data, m_sprite.zRenderPriority);
				}

				// Render the main text.
				{
					SpriteRenderData data{};
					data.color = { 255, 255, 255, 255 };
					data.position = segmentPosition;
					data.size = Utilities::to_vec2(textureDimensions);
					data.texture = query->tex;
					data.renderFlags = flags;

					_renderer->plot_texture(data, m_sprite.zRenderPriority);
				}
			}
			else 
			{
				switch(m_eRenderMode) 
				{
					case SCREENSPACE:
					{
						_renderer->plot_raw_frame(query->icon,segmentPosition, m_iconRenderSize);
					}
					break;

					case WORLDSPACE:
					{
						_renderer->plot_frame(query->icon, segmentPosition, m_iconRenderSize);
					}
					break;
				}
			}
		}
	}
}
