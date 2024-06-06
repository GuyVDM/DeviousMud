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
		struct TextSegment
		{
			std::string text;
			SDL_Color   color;
		};

		struct RenderQuery
		{
			SDL_Texture*     tex              = nullptr;
			bool             bIsIcon          = false;
			Sprite           icon;
			Utilities::vec2  renderPosOffset  = { 0.0f };

			virtual ~RenderQuery()
			{
				SDL_DestroyTexture(tex);
			}
		};

		static std::shared_ptr<TextComponent> create_text(std::string _contents, Utilities::vec2 _pos, const TextArgs _args = TextArgs::Default());

		virtual ~TextComponent();

		void set_text(std::string _contents);

		void set_text_args(const TextArgs _args = TextArgs::Default());

	protected:
		virtual void init() override;

		virtual void on_new_parent() override;

	private:
		std::vector<TextSegment> split_text_to_segments(const std::string& _contents);

		void clean();

		void rebuild_text();

		void create_texture(const std::string& _contents, const SDL_Color& _col);

		const void generate_text_textures();

		static bool parse_hex_to_color(const std::string& _string, SDL_Color& _color);

		static bool is_icon(const std::string& _contents, int32_t& _iconId);

	private:
		using UIComponent::UIComponent;

		std::vector<std::shared_ptr<RenderQuery>> m_textTextures;
		TextArgs        m_textArgs;
		std::string     m_contents;
		Utilities::vec2 m_iconRenderSize;
		std::string     m_fontPath;
		TTF_Font*       m_font;

	    void renderText(std::shared_ptr<Graphics::Renderer> _renderer);

		friend class TextComponent;
		friend class std::shared_ptr<TextComponent>;
	};
}