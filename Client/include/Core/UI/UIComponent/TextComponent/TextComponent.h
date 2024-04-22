#pragma once
#include "Core/UI/UIComponent/UIComponent.h"

#include "Core/Rendering/Fonts/FontsConfig.h"


struct TextArgs
{
	SDL_Color  color;
	int        size;
	e_FontType font;
	bool	   bDropShadow;

	TextArgs() 
	{
		color = { 255 ,255, 255, 255 };
		size = 10;
		font = e_FontType::RUNESCAPE_UF;
		bDropShadow = false;
	}

	const static TextArgs Default()
	{
		TextArgs m_textArgs{};
		m_textArgs.color = { 255, 255, 255, 255 };
		m_textArgs.size = 10;
		m_textArgs.font = e_FontType::RUNESCAPE_UF;
		m_textArgs.bDropShadow = false;
		return m_textArgs;
	}
};

struct SDL_Texture;
namespace Graphics
{
	class TextComponent final : public UIComponent
	{
	public:
		static std::shared_ptr<TextComponent> create_text(std::string _contents, Utilities::vec2 _pos, const TextArgs _args = TextArgs::Default());

		virtual ~TextComponent();

	protected:
		virtual void init() override;

	private:
		using UIComponent::UIComponent;

		SDL_Texture* m_textTexture;
		TextArgs     m_textArgs;

	    void renderText(std::shared_ptr<Graphics::Renderer> _renderer);

		friend class TextComponent;
		friend class std::shared_ptr<TextComponent>;
	};
}