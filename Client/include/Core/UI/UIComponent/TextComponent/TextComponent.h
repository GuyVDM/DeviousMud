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
		TextArgs textArgs{};
		textArgs.color = { 255, 255, 255, 255 };
		textArgs.size = 10;
		textArgs.font = e_FontType::RUNESCAPE_UF;
		textArgs.bDropShadow = false;
		return textArgs;
	}
};

struct SDL_Texture;
namespace Graphics
{
	class TextComponent final : public UIComponent
	{
	public:
		struct TextSegmentColor
		{
			std::string text;
			SDL_Color   color;
		};

		static std::shared_ptr<TextComponent> create_text(std::string _contents, Utilities::vec2 _pos, const TextArgs _args = TextArgs::Default());

		virtual ~TextComponent();

		void set_text(std::string _contents);

		void set_text_args(const TextArgs _args = TextArgs::Default());

	protected:
		virtual void init() override;

	private:
		std::vector<TextSegmentColor> split_with_color_codes(const std::string& _contents);

		void clean();

		void rebuild_text();

		void create_texture(const std::string& _contents, const SDL_Color& _col);

		const void generate_text_textures();

		bool parse_hex_to_color(const std::string& _string, SDL_Color& _color) const;

	private:
		using UIComponent::UIComponent;

		std::vector<SDL_Texture*> m_textTextures;
		TextArgs     m_textArgs;
		std::string  m_contents;

	    void renderText(std::shared_ptr<Graphics::Renderer> _renderer);

		friend class TextComponent;
		friend class std::shared_ptr<TextComponent>;
	};
}